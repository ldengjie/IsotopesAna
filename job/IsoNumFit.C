#include  <iostream>
//#include <stdio.h>
//#include  <stdlib.h>
#include  <dirent.h>//DIR
#include  <TCanvas.h>
#include  <TMath.h>
#include  <TLegend.h>
#include  <TH1F.h>
#include  <TFile.h>
#include  <fstream>
#include  <sstream>
#include  <TStyle.h>
#include "RooGlobalFunc.h"
#include "RooRealVar.h"
#include "RooConstVar.h"
#include "RooFormulaVar.h"
#include "RooChi2Var.h"
#include "RooExponential.h"
#include "RooPolynomial.h"
#include "RooMinuit.h"
#include "RooAddPdf.h"
#include "RooPlot.h"
#include "RooFitResult.h"
#include  <string>
#include  <TChain.h>
#include  <TROOT.h>
#include  <vector>
#include "RooHistPdf.h"
#include "RooExtendPdf.h"
#include "RooDataHist.h"
#include "RooChebychev.h"
#include "RooDataSet.h"
using namespace RooFit;

TString nameStr,nameStr2;
RooRealVar* xe;
RooRealVar* xt;
RooRealVar* rateMu;

typedef struct isoItems
{
    string isoName;
    bool isFixed;
    double tau_val;
    double tau_val_low;
    double tau_val_high;
    double N_value;
    double N_val_low;
    double N_val_high;
    Color_t linecolor;
    double tauInSlice[599];
    double tauErrInSlice[599];
    double numInSlice[599];
    double numErrInSlice[599];
    RooRealVar* fitTau;
    RooRealVar* fitNum;
    double num;
    double numErr;
    double rate;
    double rateErr;
    RooFormulaVar* fitLambda;
    RooExponential* fitExpPdf;
    RooExtendPdf* fitExtendPdf;
    RooHistPdf* fitHistPdf;
    /*
    isoItems()
    {
        for( int i=0 ; i<5 ; i++ )
        {
            tauInSlice[i]=0.;
            tauErrInSlice[i]=0.;
            numInSlice[i]=0.;
            numErrInSlice[i]=0.;
        }
        
    }
    */
}isoItem;
typedef struct FitInf
{
    string mode;
    bool isNoRed;
    bool isbinned;
    double elow;
    double ehigh;
    double xlow;
    double xhigh;
    double exMuonRate;
    int reBin;
    string com[10];//component
    string con;//contour
    double muonRate[599];
    double muonEnergy[599];
    double daqTime;
    double liveTime;
    map<string,isoItem> comMap;
    RooAddPdf* timeFitPdf;
    RooAddPdf* specFitPdf;
    RooDataSet* unBinnedData[599];
    RooDataHist* binnedData[599];
    int ndf;
    int sliceNum;
    double chi[599];
}fitInf;

map<string,isoItem> iso;
map<string,fitInf> fit;

void genIsoPdf(isoItem& myIso)
{
    std::cout<<Form(">>> >>> prepare %-3s information ",myIso.isoName.c_str())<<endl;
    nameStr=Form("tau%s",myIso.isoName.c_str());
    if( myIso.isFixed )
    {
        myIso.fitTau = new RooRealVar(nameStr, nameStr, -myIso.tau_val,-myIso.tau_val_high,-myIso.tau_val_low, "s");
    }else
    {
        myIso.fitTau = new RooRealVar(nameStr, nameStr, -myIso.tau_val, "s");
    }
    nameStr=Form("N_%s",myIso.isoName.c_str());
    nameStr2=Form("total number of %s",myIso.isoName.c_str());
    myIso.fitNum=new RooRealVar(nameStr,nameStr2,myIso.N_value,myIso.N_val_low,myIso.N_val_high);
    nameStr=Form("lambda%s",myIso.isoName.c_str());
    if( myIso.isoName=="Bkg" )
    {
        myIso.fitLambda=new RooFormulaVar(nameStr,nameStr,"@0 + @1",RooArgList(*myIso.fitTau, *rateMu));
    }else
    {
        myIso.fitLambda=new RooFormulaVar(nameStr,nameStr,"1/@0 + @1",RooArgList(*myIso.fitTau, *rateMu));
    }
    nameStr=Form("exp%s",myIso.isoName.c_str());
    nameStr2=Form("%s distribution",myIso.isoName.c_str());
    myIso.fitExpPdf=new RooExponential(nameStr,nameStr2, *xt, *myIso.fitLambda);

    nameStr=Form("ExtendPdf%s",myIso.isoName.c_str());
    myIso.fitExtendPdf=new RooExtendPdf(nameStr,nameStr,*myIso.fitExpPdf,*myIso.fitNum) ;
    if( myIso.isoName!="Bkg" )
    {
        TFile* f=new TFile("IsoTheoreticalSpec.root","read");
        nameStr=Form("%sSpectra",myIso.isoName.c_str());
        TH1F* h=(TH1F*)f->Get(nameStr);
        RooDataHist* hd =new RooDataHist("hd","hd",*xe,h);
        nameStr=Form("%shpdf",myIso.isoName.c_str());
        myIso.fitHistPdf=new RooHistPdf(nameStr,nameStr,*xe,*hd,2) ;
    }
}
void genFitPdf(fitInf& fitinf)
{
    RooArgList timeFitComList;
    RooArgList specFitComList;
    RooArgList specFitNumList;
    for( int i=0 ; i<10 ; i++ )
    {
        if( fitinf.com[i]!="" )
        {
            timeFitComList.add(*(iso[fitinf.com[i]].fitExtendPdf));
            fitinf.comMap.insert(map<string,isoItem>::value_type(fitinf.com[i].c_str(),iso[fitinf.com[i].c_str()]));
            if( fitinf.com[i]!="Bkg" )
            {
                specFitComList.add(*(iso[fitinf.com[i].c_str()].fitHistPdf));
                specFitNumList.add(*(iso[fitinf.com[i].c_str()].fitNum));
            }
        }
    }
    std::cout<<Form(">>> >>> prepare %-3s fit TimeFitPdf ",fitinf.mode.c_str())<<endl;
    fitinf.timeFitPdf=new RooAddPdf("timeModel","timeModel",timeFitComList) ;
    std::cout<<Form(">>> >>> prepare %-3s fit SpecFitPdf ",fitinf.mode.c_str())<<endl;
    fitinf.specFitPdf=new RooAddPdf("specMode","specMode",specFitComList,specFitNumList);
}
void calDaqTime(fitInf& fitinf,string dataVer,string site)
{
	string runnum;
	TH1F* h[5];//the fifth is for daqtime
	Double_t totalTime[5]={0.};
    double tlivetime=0.;
	string filename;
    filename.assign(dataVer);
    filename+="/";
    filename+=site;
    filename+="Time_";
    filename+=dataVer;
    filename+="_multiSlice.root";
    TFile* f=new TFile(filename.c_str());
	for( int i=0 ; i<5 ; i++ )
	{

		stringstream hname;
		if( i==4 )
		{
			hname<<"LiveTime/DaqTime";
		} else
		{
			hname<<"LiveTime/AD";
			hname<<i+1;
			hname<<"LiveTime";	
		}
		h[i] = (TH1F*)f->Get(hname.str().c_str());
		if( !(h[i]) )
		{
			cout<<"Can not get Hist : "<<hname.str()<<" from "<<site<<"/"<<runnum<<" ."<<endl;
            continue;
		}
	}
	
	TAxis *xaxis = h[0]->GetXaxis();
	int binNum = xaxis->GetNbins();
    int BinBound=h[0]->FindBin(1346428800);//2012.9.1 0:0:0
    double daqtDel=0.;
    if( BinBound!=0 )
    {
	    if( site=="EH2" )
	    {
	        for(int j=1  ; j<=BinBound ; j++ )
	        {
	            h[1]->SetBinContent(j,0);
                daqtDel+=h[4]->GetBinContent(j);
	        }
            for( int j=1 ; j<binNum ; j++ )
            {
	            h[2]->SetBinContent(j,0);
	            h[3]->SetBinContent(j,0);
            }
            
	    }else if( site=="EH3" )
	    {
	        for(int j=1  ; j<=BinBound ; j++ )
	        {
	            h[3]->SetBinContent(j,0);
                daqtDel+=h[4]->GetBinContent(j);
	        }
	    }else
        {
            for( int j=1 ; j<binNum ; j++ )
            {
	            h[2]->SetBinContent(j,0);
	            h[3]->SetBinContent(j,0);
            }
        }
    }
	for( int i=0 ; i<5 ; i++ )
	{
		totalTime[i]=0.;
		for( int j=1 ; j<=binNum ; j++ )
		{
			totalTime[i]+=h[i]->GetBinContent(j);
		}
		
	}
	for( int i=0 ; i<4 ; i++ )
	{
	    tlivetime+=totalTime[i];
	}
    fitinf.liveTime=tlivetime;
    double totalDaqtime=0.;
    if( site=="EH2" )
    {
        totalDaqtime=totalTime[4]*2-daqtDel;
    }else if( site=="EH3" )
    {
        totalDaqtime=totalTime[4]*4-daqtDel;
    } else
    {
        totalDaqtime=totalTime[4]*2;
    }
    fitinf.daqTime=totalDaqtime;
}
void genData(fitInf& fitinf, string dataVer,string site)
{
    //TODO 5 slice for time fit
    std::cout<<Form(">>> >>> prepare %-3s fit data ",fitinf.mode.c_str())<<endl;
    //nameStr=Form("%sData_%s_multiSlice.root",site.c_str(),dataVer.c_str());
    nameStr=Form("/afs/ihep.ac.cn/users/l/lidj/largedata/IsotopesAna/job/%sData_%s_multiSlice.root",site.c_str(),dataVer.c_str());
    TFile* f=new TFile(nameStr,"read");
    string noRed;
    if( fitinf.isNoRed  )
    {
        noRed="NoRed";
    } else
    {
        noRed="";
    }
    nameStr=Form("sliceNum%s_%0.2f_%0.1f_%0.1f",noRed.c_str(),fitinf.exMuonRate,fitinf.elow,fitinf.ehigh);
    std::cout<<"nameStr  : "<<nameStr<<endl;
    TH1F* h0=(TH1F*)f->Get(nameStr);
    fitinf.sliceNum=(int)(h0->GetBinContent(1));
    for( int i=2 ; i<=fitinf.sliceNum+1 ; i++ )
    {
        fitinf.muonRate[i-2]=(h0->GetBinContent(i)-(int)(h0->GetBinContent(i)/10)*10.);
        fitinf.muonEnergy[i-2]=(double)((int)(h0->GetBinContent(i)/10));
    }
    
    TH1F* h[fitinf.sliceNum];
    TH1F* h1[fitinf.sliceNum];
    TTree* t[fitinf.sliceNum];
    for( int i=0 ; i<fitinf.sliceNum ; i++ )
    {
        if( fitinf.isbinned )
        {
            
            nameStr=Form("time2lastshowermuon%s_%0.2f_%i_%0.1f_%0.1f",noRed.c_str(),fitinf.exMuonRate,i+1,fitinf.elow,fitinf.ehigh);
            std::cout<<"binned histogram  : "<<nameStr<<endl;
            h[i]=(TH1F*)f->Get(nameStr);
		    int hmax=h[i]->FindBin(fitinf.xhigh);
		    int hmin=h[i]->FindBin(fitinf.xlow);
		    int hBinNum=hmax-hmin;
		    nameStr=Form("slice%i",i+1);
		    h1[i]=new TH1F(nameStr,nameStr,hBinNum,fitinf.xlow,fitinf.xhigh);
		    for( int j=1 ; j<=hBinNum ; j++ )
		    {
		        h1[i]->SetBinContent(j,h[i]->GetBinContent(hmin+j-1));
		    }
		
		    h1[i]->SetOption("E1");
		    h1[i]->Rebin(fitinf.reBin);
		    fitinf.ndf=h1[i]->GetNbinsX()-fitinf.comMap.size()-1;
            fitinf.binnedData[i]=new RooDataHist(Form("%stime2lastmuonBinned%i",fitinf.mode.c_str(),i+1),"time2lastmuon binned data",*xt,h1[i]);
        }else
        {
	        nameStr2=Form("slice%s_%0.2f_%i_%0.1f_%0.1f",noRed.c_str(),fitinf.exMuonRate,i+1,fitinf.elow,fitinf.ehigh);
            std::cout<<"unbinned tree  : "<<nameStr2<<endl;
	        t[i]=(TTree*)f->Get(nameStr2);
            fitinf.unBinnedData[i]=new RooDataSet(Form("%stime2lastmuon%i",fitinf.mode.c_str(),i+1),"time2lastmuon unbinned data",t[i],*xt);
        }
    }

    //nameStr=Form("%sSpec_%0.1f_%0.1f",fitinf.mode.c_str(),fitinf.elow,fitinf.ehigh);
    //h[6]=(TH1F*)f->Get(nameStr);
    //fitinf.binnedData[6]=new RooDataHist(Form("%sspecBinned",fitinf.mode.c_str()),"spec binned data",*xe,h[6]);

    //f->Close();
}
void prepareInf( string dataVer,string site,string fitmode,double exRate,bool isbinned,int reBin)
{
    //fit information
    xt=new RooRealVar("xt","x for time fit",0,1);
    xe=new RooRealVar("xe","x for spec fit",0,1);
    rateMu=new RooRealVar("rateMu","rateMu",1);
    //isotopes information
    isoItem isoB12={"B12",0,0.0291,0.011,0.8 ,2.e4,0,5.e6,632};
    isoItem isoN12={"N12",0,0.0159,0.005,0.08,2.e4,0,5.e6,635};
    isoItem isoLi8={"Li8",0,1.21  ,0.511,5.0 ,2.e4,0,5.e6,601};
    isoItem isoB8 ={"B8" ,0,1.11  ,0.511,5.0 ,2.e4,0,5.e6,909};
    isoItem isoC9 ={"C9" ,0,0.1825,0.051,0.5 ,2.e4,0,5.e6,800};
    isoItem isoLi9={"Li9",0,0.1717,0.081,0.8 ,2.e4,0,5.e6,415};
    isoItem isoHe8={"He8",0,0.2572,0.111,0.8 ,2.e4,0,5.e6,432};
    isoItem isoBkg={"Bkg",1,0.5   ,0.   ,50. ,2.e4,0,5.e6,416};
    iso.insert(map<string,isoItem>::value_type("B12",isoB12));
    iso.insert(map<string,isoItem>::value_type("N12",isoN12));
    iso.insert(map<string,isoItem>::value_type("Li8",isoLi8));
    iso.insert(map<string,isoItem>::value_type("B8" ,isoB8));
    iso.insert(map<string,isoItem>::value_type("C9" ,isoC9));
    iso.insert(map<string,isoItem>::value_type("Li9",isoLi9));
    iso.insert(map<string,isoItem>::value_type("He8",isoHe8));
    iso.insert(map<string,isoItem>::value_type("Bkg",isoBkg));
    std::cout<<"done iso "<<endl;
    fitInf fitB12={"B12",1,isbinned,4.0 ,20.0,0.001,0.501,exRate,reBin,{"B12","N12","C9","He8","Li9","Li8","B8","Bkg"},"N12"};
    fitInf fitN12={"N12",1,isbinned,14.0,20.0,0.001,0.501,exRate,reBin ,{"N12","C9","Li8","B8","Bkg"},"C9"};
    fitInf fitLi8={"Li8",1,isbinned,4.0 ,20.0,0.8  ,10.  ,exRate,reBin,{"Li8","B8","C9","Bkg"},"B8"};
    fitInf fitC9 ={"C9" ,1,isbinned,12.0,20.0,0.15 ,2.0  ,exRate,reBin,{"C9" ,"He8","B8","Li8","Bkg"},"Li8"};
    fit.insert(map<string,fitInf>::value_type("B12",fitB12));
    fit.insert(map<string,fitInf>::value_type("N12",fitN12));
    fit.insert(map<string,fitInf>::value_type("Li8",fitLi8));
    fit.insert(map<string,fitInf>::value_type("C9",fitC9));
    std::cout<<"done fit "<<endl;
    std::cout<<"fitmode  : "<<fitmode<<endl;
    std::cout<<"fit[fitmode].xlow  : "<<fit[fitmode].xlow<<endl;
    std::cout<<"fit[fitmode].xhigh  : "<<fit[fitmode].xhigh<<endl;
    std::cout<<"fit[fitmode].elow  : "<<fit[fitmode].elow<<endl;
    std::cout<<"fit[fitmode].ehigh  : "<<fit[fitmode].ehigh<<endl;
    xt->setRange(fit[fitmode].xlow,fit[fitmode].xhigh);
    //xt->setRange(1,5);
    std::cout<<"done xt "<<endl;
    xe->setRange(fit[fitmode].elow,fit[fitmode].ehigh);
    std::cout<<"done setrange "<<endl;
    for( int i=0 ; i<10 ; i++ )
    {
       if( fit[fitmode].com[i]!="" )
       {
          genIsoPdf(iso[fit[fitmode].com[i]]); 
       }
    }
    
    genFitPdf(fit[fitmode]);calDaqTime(fit[fitmode],dataVer,site);genData(fit[fitmode],dataVer,site);
}


int doFit(int siteNum,string dataVer,string fitMode,double exRate,bool isbinned,int reBin,double fitLowRange,double fitHighRange)
{

//===>initialize variable
	bool anaIso=1;//N12/B12 bg
    //bool anaE=0;
	string site;
	if( siteNum==1 )
	{
		site="EH1";
	} else if(siteNum==2)
	{
		site="EH2";
	}else if (siteNum==3)
	{
		site="EH3";
	}else
	{
		site="EH1";
	}


//===> fit iso
    std::cout<<"begin to analyse B12 "<<endl;
	if(anaIso)
	{
        TString xTitle[6]={"20~60MeV","60~500MeV","0.5~1.5GeV","1.5~2.5GeV",">2.5GeV","all muon"};
        //double showerTh[6] = {0.02, 0.06, 0.5, 1.5, 2.5, 5.0};
        //B12Result[0]= new TH1F("B12Yield", "All", 5, showerTh);
        //B12Result[1]= new TH1F("B12YieldAD1", "AD1", 5, showerTh);
        //B12Result[2]= new TH1F("B12YieldAD2", "AD2", 5, showerTh);
        //B12Result[3]= new TH1F("B12YieldAD3", "AD3", 5, showerTh);
        //B12Result[4]= new TH1F("B12YieldAD4", "AD4", 5, showerTh);
        
        prepareInf(dataVer,site,fitMode,exRate,isbinned,reBin);   
        bool draw6slices=0;
        if( fitLowRange!=0 && fitHighRange!=0 )
        {
            fit[fitMode].xlow=fitLowRange;
            fit[fitMode].xhigh=fitHighRange;
        }
        std::cout<<"xt range  : "<<fit[fitMode].xlow<<" ~ "<<fit[fitMode].xhigh <<endl;

		RooFitResult* fitres;

        TCanvas* c;
        if( draw6slices )
        {
            c = new TCanvas("c","c",1200,600) ; 
            c->Divide(3,2);
            gStyle->SetEndErrorSize(5.0);
            gStyle->SetMarkerSize(0.1);
        }
        for( int ihist=0 ; ihist<1 ; ihist++ )
        {
			for( int j=0 ; j<fit[fitMode].sliceNum ; j++ )
			{
                std::cout<<">>>>>>>>>>>> now is  : "<<j+1<<endl;
                rateMu->setVal(-fit[fitMode].muonRate[j]);
                //Num_tot[j]=hh[j]->Integral(1,hh[j]->FindBin(fit[fitMode].xhigh));
                if( fit[fitMode].isbinned  )
                {
                    fit[fitMode].timeFitPdf->fitTo(*(fit[fitMode].binnedData[j]),Save(),PrintLevel(-1));
	                RooAbsReal* nll = fit[fitMode].timeFitPdf->createNLL(*(fit[fitMode].binnedData[j])) ;
	                RooMinuit m(*nll) ; 
	                m.migrad() ; 
	                m.hesse() ;
	                m.contour(*(iso[fit[fitMode].con].fitNum),*(iso[fitMode].fitNum),1,2,3) ; 
                }
                if( fit[fitMode].isbinned )
                {
                    fitres = fit[fitMode].timeFitPdf->fitTo(*(fit[fitMode].binnedData[j]),Save(),PrintLevel(-1));
                    RooChi2Var* chi2=new RooChi2Var("chi2", "chi2", *(fit[fitMode].timeFitPdf), *(fit[fitMode].binnedData[j]));
                    fit[fitMode].chi[j]=chi2->getVal();
                }else
                {
                    fitres = fit[fitMode].timeFitPdf->fitTo(*(fit[fitMode].unBinnedData[j]),Save(),PrintLevel(-1),NumCPU(20));
                }
                //fitres->Print();

                for( map<string,isoItem>::iterator it=fit[fitMode].comMap.begin() ; it!=fit[fitMode].comMap.end() ; it++ )
                {
                   it->second.numInSlice[j]=it->second.fitNum->getVal(0); 
                   it->second.numErrInSlice[j]=it->second.fitNum->getError(); 
                   it->second.tauInSlice[j]=it->second.fitTau->getVal(0);
                   it->second.tauErrInSlice[j]=it->second.fitTau->getError();
                   if( j!=5)
                   {
                    it->second.num+=it->second.fitNum->getVal(0);
                    it->second.numErr+=it->second.fitNum->getError()*it->second.fitNum->getError();
                   }
                }
	
               if( draw6slices )
                {
		            c->cd(j+1);
		            RooPlot* mesframe = xt->frame() ;
		            xTitle[j]+="        time since last muon (s)";
		            mesframe->GetXaxis()->SetTitle(xTitle[j]);
		            mesframe->GetYaxis()->SetTitle("Entries");
                    TLegend* leg = new TLegend(0.3,0.6,0.95,0.94);
                    leg->SetTextSize(0.05);

                    if( fit[fitMode].isbinned )
                    {
                        fit[fitMode].binnedData[j]->plotOn(mesframe) ;
                    } else
                    {
                        fit[fitMode].unBinnedData[j]->plotOn(mesframe) ;
                    }
                    fit[fitMode].timeFitPdf->plotOn(mesframe,Name("sum"));
                    leg->SetHeader(Form("---%s Fit---",fitMode.c_str()));
                    for(map<string,isoItem>::iterator it=fit[fitMode].comMap.begin();it!=fit[fitMode].comMap.end() ; it++)
                    {
                        fit[fitMode].timeFitPdf->plotOn(mesframe,Components(*(it->second.fitExpPdf)),Name(Form("%s",it->second.isoName.c_str())),LineStyle(kDashed),LineColor(it->second.linecolor)) ;
                        leg->AddEntry(mesframe->findObject(Form("%s",it->second.isoName.c_str())),Form("%-3s ~ %0.2f",it->second.isoName.c_str(),it->second.numInSlice[j]),"l");
                    }
                    if( fit[fitMode].isbinned )
                    {
                        TString chiStr = Form("chi/ndf  =  %2.2f / %i = %2.2f", fit[fitMode].chi[j],fit[fitMode].ndf,fit[fitMode].chi[j]/fit[fitMode].ndf);
                        leg->AddEntry((TObject*)0,chiStr,"");
                    }
                    leg->SetFillColor(10);
		            mesframe->Draw();
                    leg->Draw();
                    gPad->SetLogy();
                }
			}
	        
            std::cout<<" "<<endl;
            std::cout<<"livetime  : "<<fit[fitMode].liveTime/(24*3600)<<endl;

            std::cout<<" Slice |";
            //for(  map<string,isoItem>::iterator it=fit[fitMode].comMap.begin() ; it!=fit[fitMode].comMap.end() ; it++ )
            for( int k=0;k<(int)(fit[fitMode].comMap.size());k++) 
            {
                std::cout<<Form("    %-6s |",iso[fit[fitMode].com[k]].isoName.c_str());
            }
            std::cout<<" MuonRate | MuonEnergy |";
            if( fit[fitMode].isbinned )
            {
                std::cout<<"    chi/ndf ";
            }
            std::cout<<endl;

	        for( int j=0 ; j<fit[fitMode].sliceNum ; j++ )
	        {
                std::cout<<Form(" %5i |",j);//"│" | | ｜｜
                for( int k=0;k<(int)(fit[fitMode].comMap.size());k++) 
                {
                    std::cout<<Form(" %9.1f |",fit[fitMode].comMap[fit[fitMode].com[k]].numInSlice[j]);//can't use "iso[fit[fitMode].com[k]]",not is reference type.
                }
                std::cout<<Form(" %8.5f |",fit[fitMode].muonRate[j]);
                std::cout<<Form(" %10.0f |",fit[fitMode].muonEnergy[j]);
                if( fit[fitMode].isbinned )
                {
                    std::cout<<Form(" %8.2f/%i= %5.2f",fit[fitMode].chi[j],fit[fitMode].ndf,fit[fitMode].chi[j]/fit[fitMode].ndf); 
                }
                std::cout<<endl;
                
	        }
            std::cout<<"      |       Total number      |     Rate(/day/AD) "<<endl;
            for( int k=0;k<(int)(fit[fitMode].comMap.size());k++) 
            {
                fit[fitMode].comMap[fit[fitMode].com[k]].rate=fit[fitMode].comMap[fit[fitMode].com[k]].num/(fit[fitMode].liveTime/(24*3600));
                fit[fitMode].comMap[fit[fitMode].com[k]].rateErr=sqrt(fit[fitMode].comMap[fit[fitMode].com[k]].numErr)/(fit[fitMode].liveTime/(24*3600));
	            std::cout<<Form(" %-3s  | %10.1f +- %8.1f  | %7.1f +- %7.1f",fit[fitMode].comMap[fit[fitMode].com[k]].isoName.c_str(),fit[fitMode].comMap[fit[fitMode].com[k]].num,sqrt(fit[fitMode].comMap[fit[fitMode].com[k]].numErr),fit[fitMode].comMap[fit[fitMode].com[k]].rate,fit[fitMode].comMap[fit[fitMode].com[k]].rateErr)<<endl;
            }
        }

        /*
        //if( draw6slices )
        //{
            //c->cd(6);
            //std::cout<<"cd 6 "<<endl;
            //std::cout<<"1  : "<<B12Result[0]->GetBinContent(1)<<endl;
            //std::cout<<"2  : "<<B12Result[0]->GetBinContent(2)<<endl;
            //std::cout<<"3  : "<<B12Result[0]->GetBinContent(3)<<endl;
            //std::cout<<"4  : "<<B12Result[0]->GetBinContent(4)<<endl;
            //std::cout<<"5  : "<<B12Result[0]->GetBinContent(5)<<endl;
            //B12Result[0]->Draw();
            //}
        //B12Result[0]->SetMarkerStyle(20);
		//B12Result[0]->SetMarkerColor(lcolor[0]);
		//B12Result[0]->SetLineColor(lcolor[0]);
		//B12Result[0]->SetMarkerSize(1.0);
		//B12Result[0]->SetMinimum(0);
		//B12Result[0]->SetMaximum(B12Result[0]->GetMaximum()*1.5);
		//B12Result[0]->GetXaxis()->SetTitle("Muon visible energy (GeV)");
		//B12Result[0]->GetYaxis()->SetTitle("^{12}B/^{12}N yield per muon");
		//B12Result[0]->SetTitle("");
        //B12Result[0]->SetStats(kFALSE);
		//B12Result[0]->Draw("EP");
        TLegend *legend=new TLegend(.6,.65,.79,.89);
        TString legendLabel=dataVer+" "+site+" "+"All";
        legend->AddEntry(B12Result[0],legendLabel,"lp");

        for( int ihist=1 ; ihist<ADNumOfSite[siteNum-1]+1; ihist++ )
        {
            B12Result[ihist]->SetMarkerStyle(20);
		    B12Result[ihist]->SetMarkerColor(lcolor[ihist]);
		    B12Result[ihist]->SetLineColor(lcolor[ihist]);
		    B12Result[ihist]->SetMarkerSize(1.0);
            B12Result[ihist]->SetStats(kFALSE);
		    B12Result[ihist]->Draw("EPsame");
            legend->AddEntry(B12Result[ihist],lname[ihist],"lp");
        }
        legend->SetFillColor(0);
        legend->Draw();
        */
	}	
	

//===>write into .root .text file

    nameStr=Form("/afs/ihep.ac.cn/users/l/lidj/largedata/IsotopesAna/job/%s_%s_%s_%0.2f_%i_%i.root",dataVer.c_str(),site.c_str(),fitMode.c_str(),fit[fitMode].exMuonRate,fit[fitMode].isbinned,fit[fitMode].reBin);
    TFile* file = new TFile(nameStr,"RECREATE");
    double eBin[599]={0.};
    eBin[0]=20.;
    for( int i=0 ; i<fit[fitMode].sliceNum ; i++ )
    {
        eBin[i+1]=fit[fitMode].muonEnergy[i];
    }
    
    if( fit[fitMode].isbinned )
    {
        nameStr=Form("%s_binnedFit_chi",fitMode.c_str());
        TH1F* h=new TH1F(nameStr,nameStr,fit[fitMode].sliceNum,eBin);
        for( int i=0 ; i<fit[fitMode].sliceNum ; i++ )
        {
            h->SetBinContent(i+1,fit[fitMode].chi[i]/fit[fitMode].ndf);
        }
        h->Write();
        delete h;

    }
    nameStr="muonRate";
    nameStr2="muonRate in each slice";
    TH1F* h1=new TH1F(nameStr,nameStr2,fit[fitMode].sliceNum,eBin);
    for( int i=0 ; i<fit[fitMode].sliceNum ; i++ )
    {
        h1->SetBinContent(i+1,fit[fitMode].muonRate[i]);
    }
    h1->Write();
    delete h1;

    for( int k=0;k<(int)(fit[fitMode].comMap.size());k++) 
    {
        nameStr=Form("%s",fit[fitMode].com[k].c_str());
        nameStr2=Form("%s entries in each slice",fit[fitMode].com[k].c_str());
        TH1F* h=new TH1F(nameStr,nameStr2,fit[fitMode].sliceNum,eBin);
        for( int i=0 ; i<fit[fitMode].sliceNum ; i++ )
        {
            h->SetBinContent(i+1,fit[fitMode].comMap[fit[fitMode].com[k]].numInSlice[i]);
            h->SetBinError(i+1,fit[fitMode].comMap[fit[fitMode].com[k]].numErrInSlice[i]);
        }
        h->Write();
        delete h;
    }
    
    file->Close();

	ofstream outfile;
    nameStr=Form("/afs/ihep.ac.cn/users/l/lidj/largedata/IsotopesAna/job/%s_%s_%s_%0.2f_%i_%i.txt",dataVer.c_str(),site.c_str(),fitMode.c_str(),fit[fitMode].exMuonRate,fit[fitMode].isbinned,fit[fitMode].reBin);
	outfile.open(nameStr);
    outfile<<" "<<endl;
    outfile<<"livetime  : "<<fit[fitMode].liveTime/(24*3600)<<endl;

    outfile<<" Slice |";
    //for(  map<string,isoItem>::iterator it=fit[fitMode].comMap.begin() ; it!=fit[fitMode].comMap.end() ; it++ )
    for( int k=0;k<(int)(fit[fitMode].comMap.size());k++) 
    {
        outfile<<Form("    %-6s |",iso[fit[fitMode].com[k]].isoName.c_str());
    }
    outfile<<" MuonRate | MuonEnergy |";
    if( fit[fitMode].isbinned )
    {
        outfile<<"    chi/ndf ";
    }
    outfile<<endl;

	for( int j=0 ; j<fit[fitMode].sliceNum ; j++ )
	{
        outfile<<Form(" %5i |",j);//"│" | | ｜｜
        for( int k=0;k<(int)(fit[fitMode].comMap.size());k++) 
        {
            outfile<<Form(" %9.1f |",fit[fitMode].comMap[fit[fitMode].com[k]].numInSlice[j]);//can't use "iso[fit[fitMode].com[k]]",not is reference type.
        }
        outfile<<Form(" %8.5f |",fit[fitMode].muonRate[j]);
        outfile<<Form(" %10.0f |",fit[fitMode].muonEnergy[j]);
        if( fit[fitMode].isbinned )
        {
            outfile<<Form(" %8.2f/%i= %5.2f",fit[fitMode].chi[j],fit[fitMode].ndf,fit[fitMode].chi[j]/fit[fitMode].ndf); 
        }
        outfile<<endl;
        
	}
    outfile<<"      |       Total number      |     Rate(/day/AD) "<<endl;
    for( int k=0;k<(int)(fit[fitMode].comMap.size());k++) 
    {
        fit[fitMode].comMap[fit[fitMode].com[k]].rate=fit[fitMode].comMap[fit[fitMode].com[k]].num/(fit[fitMode].liveTime/(24*3600));
        fit[fitMode].comMap[fit[fitMode].com[k]].rateErr=sqrt(fit[fitMode].comMap[fit[fitMode].com[k]].numErr)/(fit[fitMode].liveTime/(24*3600));
	    outfile<<Form(" %-3s  | %10.1f +- %8.1f  | %7.1f +- %7.1f",fit[fitMode].comMap[fit[fitMode].com[k]].isoName.c_str(),fit[fitMode].comMap[fit[fitMode].com[k]].num,sqrt(fit[fitMode].comMap[fit[fitMode].com[k]].numErr),fit[fitMode].comMap[fit[fitMode].com[k]].rate,fit[fitMode].comMap[fit[fitMode].com[k]].rateErr)<<endl;
    }
	outfile.close();
	return 0;
}

vector<string> checkdata(string dataVer)
{
    string dataIsGood="1";
    string verNum[6]={"11","12","13","14","15","16"};
    vector<string> dataVerReal;
    for( int i=0 ; i<6 ; i++ )
    {
        int pos=0;
        string TmpVer;
        TmpVer=dataVer.substr(pos);
        while( pos!=-1 )
        {
            pos=TmpVer.find(verNum[i]);
            if( pos!=-1 )
            {
                string subdataVer;
                subdataVer="P"+verNum[i];
                subdataVer+=toupper(*(TmpVer.substr(pos+2,1).c_str()));
                for( vector<string>::iterator it=dataVerReal.begin() ; it!=dataVerReal.end() ; ++it  )
                {
                    if( *it==subdataVer )
                    {
                        dataVerReal.erase(it);
                        it--;
                    }
                }
                dataVerReal.push_back(subdataVer);
                TmpVer=TmpVer.substr(pos+3);
            }
        }
    }
    sort(dataVerReal.begin(),dataVerReal.end());

    std::cout<<" "<<endl;
    std::cout<<"Find following data version will be analysed : "<<endl;
    std::cout<<" "<<endl;
    string runlistSiteNum[3]={"EH1","EH2","EH3"};
    vector<string> runlistFileContent[3];
    for( int i=0 ; i<3 ; i++ )
    {
        runlistFileContent[i].clear();
    }
    string verSuf;
    for( vector<string>::iterator it=dataVerReal.begin() ; it!=dataVerReal.end() ; ++it )
    {
        std::cout<<*it<<endl;
        verSuf+="_"+*it;
        //check data path
        string datadir="/afs/ihep.ac.cn/users/l/lidj/largedata/IsotopesAna/";
        DIR *dir=NULL;
        datadir+=*it;
        dir = opendir(datadir.c_str());
        if( dir==NULL )
        {
            std::cout<<" !!! data dir doesn't exist : "<<datadir<<endl;
            dataIsGood="0";
        } else
        {
            std::cout<<" data dir exist : "<<datadir<<endl;
            closedir(dir);
        }

        
        std::cout<<" "<<endl;
    }
        //check merged file 
        for( int i=0 ; i<3 ; i++ )
        {
            string runlistName;
            runlistName.assign(verSuf);
            runlistName+="/";
            runlistName+=runlistSiteNum[i];
            runlistName+="Time";
            runlistName+=verSuf;
            //runlistName+=*it;
            runlistName+="_multiSlice.root";
            TFile *ff = new TFile(runlistName.c_str());
            if( ff->IsZombie() )
            {
                std::cout<<" !!! merged file doesn't exist  : "<<runlistName<<endl;
                /*
                if( *it!="P12A" )
                {
                    dataIsGood="0";
                }
                */
            } else
            {
                std::cout<<" merged file exist  : "<<runlistName<<endl;
            }
            ff->Close();
        }
    verSuf=verSuf.substr(1);
    std::cout<<"data version : "<<verSuf<<endl;

    dataVerReal.push_back(verSuf);
    dataVerReal.push_back(dataIsGood);
    
    return dataVerReal; 
}

//int IsoNumFit(string dataVer,int siteNum=0,string FitMode="B12",double FitLowRange=0.,double FitHighRange=0.)
int main(int argc, char *argv[])
{
    string dataVer;
    int siteNum=0;
    string FitMode;
    double exRate=0.;
    int isbinned=0;
    int reBin=0;
    double FitLowRange=0.;
    double FitHighRange=0.;
    dataVer=argv[1];
    siteNum=atoi(argv[2]);
    FitMode=argv[3];
    exRate=atof(argv[4]);
    isbinned=atoi(argv[5]);
    reBin=atoi(argv[6]);
    if( argc==9 )
    {
        FitLowRange=atof(argv[7]);
        std::cout<<"FitLowRange  : "<<FitLowRange<<endl;
        FitHighRange=atof(argv[8]);
        std::cout<<"FitHighRange  : "<<FitHighRange<<endl;
    }
    vector<string> dataVerVec=checkdata(dataVer);
    std::cout<<" "<<endl;
    std::cout<<" "<<endl;
    std::cout<<" "<<endl;
    std::cout<<" "<<endl;
    dataVer=dataVerVec[dataVerVec.size()-2];
    if( dataVerVec[dataVerVec.size()-1]=="0" )
    {
        std::cout<<"  The dataVerion["<<dataVer<<"] or data is wrong ,please check! "<<endl;
        std::cout<<" "<<endl;
        std::cout<<" "<<endl;
        std::cout<<" "<<endl;
        std::cout<<" "<<endl;
        return 0;
    }

    if( siteNum==0 )
    {
        for( int i=1 ; i<=3 ; i++ )
        {
            std::cout<<"====> begin to analyse EH"<<i<<"'s DaqTime, N12/B12 "<<endl;
            std::cout<<"dataVersion  : "<<dataVer<<endl;
            doFit(i,dataVer,FitMode,exRate,isbinned,reBin,FitLowRange,FitHighRange);
            std::cout<<" "<<endl;
            std::cout<<" "<<endl;
            std::cout<<" "<<endl;
            std::cout<<" "<<endl;
        }
        
    }else if(siteNum>=1 && siteNum<=3)
    {
        std::cout<<"====> begin to analyse EH"<<siteNum<<"'s DaqTime,N12/B12 "<<endl;
        std::cout<<"dataVersion  : "<<dataVer<<endl;
        doFit(siteNum,dataVer,FitMode,exRate,isbinned,reBin,FitLowRange,FitHighRange);
        std::cout<<" "<<endl;
        std::cout<<" "<<endl;
        std::cout<<" "<<endl;
        std::cout<<" "<<endl;
    }
    std::cout<<"ALL DONE!!! "<<endl;
    std::cout<<" "<<endl;
    std::cout<<" "<<endl;
    std::cout<<" "<<endl;
    return 0;
}

