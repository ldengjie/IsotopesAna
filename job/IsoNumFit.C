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
#include  "RooSimultaneous.h"
#include  "RooAbsReal.h"
#include "RooConstVar.h"
#include "RooFormulaVar.h"
#include "RooChi2Var.h"
#include "RooExponential.h"
#include "RooPolynomial.h"
#include "RooMinuit.h"
#include "RooAddPdf.h"
#include "RooPlot.h"
#include "RooFitResult.h"
#include  "RooCategory.h"
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
    double tauInSlice[6];
    double tauErrInSlice[6];
    double tNumInSlice[6];
    double tNumErrInSlice[6];
    double tNum;
    double tNumErr;
    double eNumInSlice[6];
    double eNumErrInSlice[6];
    double eNum;
    double eNumErr;
    double rate;
    double rateErr;
    RooRealVar* fitTau;
    RooRealVar* tFitNum;
    RooFormulaVar* fitLambda;
    RooExponential* fitExpPdf;
    RooRealVar* tCutCoe;
    RooExtendPdf* fitExtendPdf;
    RooHistPdf* fitHistPdf;
    RooRealVar* eCutCoe;
    RooFormulaVar* eFitNum;
    /*
       isoItems()
       {
       for( int i=0 ; i<5 ; i++ )
       {
       tauInSlice[i]=0.;
       tauErrInSlice[i]=0.;
       tNumInSlice[i]=0.;
       tNumErrInSlice[i]=0.;
       }

       }
     */
}isoItem;
typedef struct FitInf
{
    string mode;
    bool isNoRed;
    bool isbinned;
    bool doSimulFit;
    double elow;
    double ehigh;
    double xlow;
    double xhigh;
    string com[10];//component
    string con;//contour
    double muonRate[7];
    map<string,isoItem*> comMap;
    RooAddPdf* timeFitPdf;
    RooAddPdf* specFitPdf;
    RooDataSet* unBinnedData[7];
    RooDataHist* binnedData[12];
    int ndf;
    double chi[7];
    string ifRed;
}fitInf;

map<string,isoItem*> iso;
map<string,fitInf> fit;

void genIsoPdf(isoItem* myIso,double _xlow,double _xhigh,double _elow,double _ehigh)
{
    std::cout<<Form(">>> >>> prepare %-3s information ",myIso->isoName.c_str())<<endl;
    nameStr=Form("tau%s",myIso->isoName.c_str());
    if( myIso->isFixed )
    {
        myIso->fitTau = new RooRealVar(nameStr, nameStr, -myIso->tau_val,-myIso->tau_val_high,-myIso->tau_val_low, "s");
    }else
    {
        myIso->fitTau = new RooRealVar(nameStr, nameStr, -myIso->tau_val, "s");
    }
    nameStr=Form("N_%s",myIso->isoName.c_str());
    nameStr2=Form("total number of %s",myIso->isoName.c_str());
    myIso->tFitNum=new RooRealVar(nameStr,nameStr2,myIso->N_value,myIso->N_val_low,myIso->N_val_high);
    nameStr=Form("lambda%s",myIso->isoName.c_str());
    if( myIso->isoName=="Bkg" )
    {
        myIso->fitLambda=new RooFormulaVar(nameStr,nameStr,"@0 + @1",RooArgList(*myIso->fitTau, *rateMu));
    }else
    {
        myIso->fitLambda=new RooFormulaVar(nameStr,nameStr,"1/@0 + @1",RooArgList(*myIso->fitTau, *rateMu));
    }
    nameStr=Form("exp%s",myIso->isoName.c_str());
    nameStr2=Form("%s distribution",myIso->isoName.c_str());
    myIso->fitExpPdf=new RooExponential(nameStr,nameStr2, *xt, *myIso->fitLambda);
    //RooAbsReal* tTmpCoe =myIso->fitExpPdf->createIntegral(*xt,NormSet(*xt));
    xt->setRange("tRange",_xlow,_xhigh);
    //RooAbsReal* tTmpCoe =myIso->fitExpPdf->createIntegral(*xt,Range(0.001,0.201));
    RooAbsReal* tTmpCoe =myIso->fitExpPdf->createIntegral(*xt,NormSet(*xt),Range("tRange"));
    std::cout<<"tTmpCoe  : "<<tTmpCoe->getVal()<<endl;
    //double tTmpCoe =myIso->fitExpPdf->getVal(xt);
    nameStr=Form("%stCutCoe",myIso->isoName.c_str());
    myIso->tCutCoe=new RooRealVar(nameStr,nameStr,tTmpCoe->getVal());
    //std::cout<<"tTmpCoe  : "<<tTmpCoe<<endl;
    delete tTmpCoe;
    nameStr=Form("ExtendPdf%s",myIso->isoName.c_str());
    myIso->fitExtendPdf=new RooExtendPdf(nameStr,nameStr,*myIso->fitExpPdf,*(myIso->tFitNum)) ;
    if( myIso->isoName!="Bkg" )
    {
        TFile* f=new TFile("IsoTheoreticalSpec.root","read");
        nameStr=Form("%sSpectra",myIso->isoName.c_str());
        TH1F* h=(TH1F*)f->Get(nameStr);
        RooDataHist* hd =new RooDataHist("hd","hd",*xe,h);
        nameStr=Form("%shpdf",myIso->isoName.c_str());
        myIso->fitHistPdf=new RooHistPdf(nameStr,nameStr,*xe,*hd,2) ;
        xe->setRange("eRange",_elow,_ehigh);
        //RooAbsReal* eTmpCoe =myIso->fitHistPdf->createIntegral(*xe,NormSet(*xe));
        RooAbsReal* eTmpCoe =myIso->fitHistPdf->createIntegral(*xe,NormSet(*xe),Range("eRange"));
        nameStr=Form("%seCutCoe",myIso->isoName.c_str());
        myIso->eCutCoe=new RooRealVar(nameStr,nameStr,eTmpCoe->getVal());
        std::cout<<"eTmpCoe  : "<<eTmpCoe->getVal()<<endl;
        delete eTmpCoe;
        //myIso->eCutCoe=myIso->fitHistPdf->createIntegral(*xe,NormSet(*xe));
        nameStr=Form("%seFitNum",myIso->isoName.c_str());
        myIso->eFitNum=new RooFormulaVar(nameStr,nameStr,"@0/@1*@2",RooArgList(*(myIso->tFitNum),*(myIso->tCutCoe),*(myIso->eCutCoe)));
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
            timeFitComList.add(*(iso[fitinf.com[i]]->fitExtendPdf));
            std::cout<<"iso["<<fitinf.com[i].c_str()<<"]  : "<<iso[fitinf.com[i].c_str()]<<endl;
            std::cout<<"iso["<<fitinf.com[i].c_str()<<"]->isoName  : "<<iso[fitinf.com[i].c_str()]->isoName<<endl;
            fitinf.comMap.insert(map<string,isoItem*>::value_type(fitinf.com[i].c_str(),iso[fitinf.com[i].c_str()]));
            std::cout<<"comMap["<<fitinf.com[i].c_str() <<"]->isoName  : "<<fitinf.comMap[fitinf.com[i].c_str()]->isoName<<endl;
            if( fitinf.com[i]!="Bkg" )
            {
                specFitComList.add(*(iso[fitinf.com[i].c_str()]->fitHistPdf));
                specFitNumList.add(*(iso[fitinf.com[i].c_str()]->eFitNum));
            }
        }
    }
    std::cout<<Form(">>> >>> prepare %-3s fit TimeFitPdf ",fitinf.mode.c_str())<<endl;
    fitinf.timeFitPdf=new RooAddPdf("timeModel","timeModel",timeFitComList) ;
    std::cout<<Form(">>> >>> prepare %-3s fit SpecFitPdf ",fitinf.mode.c_str())<<endl;
    fitinf.specFitPdf=new RooAddPdf("specMode","specMode",specFitComList,specFitNumList);
}

void genData(fitInf& fitinf, string dataVer,string site)
{
    //TODO 5 slice for time fit
    std::cout<<Form(">>> >>> prepare %-3s fit data ",fitinf.mode.c_str())<<endl;
    nameStr=Form("%s/%siso_%s.root",dataVer.c_str(),site.c_str(),dataVer.c_str());
    TFile* f=new TFile(nameStr,"read");
    TH1F* h[6];
    TH1F* hs[6];
    TH1F* ht[7];
    TTree* t[7];
    if( fitinf.isNoRed )
    {
        fitinf.ifRed="NoRed";
    } else
    {
        fitinf.ifRed="";
    }
    for( int i=0 ; i<6 ; i++ )
    {
        if( fitinf.isbinned )
        {
            nameStr=Form("time2lastshowermuon%s%i_%0.1f_%0.1f",fitinf.ifRed.c_str(),i+1,fitinf.elow,fitinf.ehigh);
            if( i==5 )
            {
                nameStr=Form("time2Allmuon%s_%0.1f_%0.1f",fitinf.ifRed.c_str(),fitinf.elow,fitinf.ehigh); 
            }
            h[i]=(TH1F*)f->Get(nameStr);
            nameStr2=Form("%sSpec%sSlice%i_%0.1f_%0.1f",fitinf.mode.c_str(),fitinf.ifRed.c_str(),i+1,fitinf.elow,fitinf.ehigh);
            hs[i]=(TH1F*)f->Get(nameStr2);
            //for( int j=hs[i]->FindBin(fitinf.elow) ; j<(hs[i]->FindBin(fitinf.ehigh)-hs[i]->FindBin(fitinf.elow)) ; j++ )
            //{
            //std::cout<<"spec bin "<<j <<"  : "<<hs[i]->GetBinContent(j)<<endl;
            // 
            //}

            int hmax=h[i]->FindBin(fitinf.xhigh);
            int hmin=h[i]->FindBin(fitinf.xlow);
            int hBinNum=hmax-hmin;
            std::cout<<"hBinNum  : "<<hBinNum<<endl;
            nameStr=Form("slice%i",i+1);
            ht[i]=new TH1F(nameStr,nameStr,hBinNum,fitinf.xlow,fitinf.xhigh);
            for( int j=1 ; j<=hBinNum ; j++ )
            {
                ht[i]->SetBinContent(j,h[i]->GetBinContent(hmin+j-1));
            }

            ht[i]->SetOption("E1");
            ht[i]->Rebin(5);
            fitinf.ndf=ht[i]->GetNbinsX()-fitinf.comMap.size()-1;
            fitinf.binnedData[i]=new RooDataHist(Form("%stime2lastmuonBinned",fitinf.mode.c_str()),"time2lastmuon binned data",*xt,ht[i]);
            fitinf.binnedData[i+6]=new RooDataHist(Form("%sspecBinned",fitinf.mode.c_str()),"spec binned data",*xe,hs[i]);
        }else
        {
            nameStr2=Form("slice%s%i_%0.1f_%0.1f",fitinf.ifRed.c_str(),i+1,fitinf.elow,fitinf.ehigh);
            if( i==5 )
            {
                nameStr2=Form("slice%s6_%0.1f_%0.1f",fitinf.ifRed.c_str(),fitinf.elow,fitinf.ehigh);
            }
            t[i]=(TTree*)f->Get(nameStr2);
            fitinf.unBinnedData[i]=new RooDataSet(Form("%stime2lastmuon",fitinf.mode.c_str()),"time2lastmuon unbinned data",t[i],*xt);
        }
    }

    //f->Close();
}

    isoItem isoB12={"B12",0,0.0291,0.011,0.8 ,1.e4,0,5.e6,632};
    isoItem isoN12={"N12",0,0.0159,0.005,0.08,1.e4,0,5.e6,635};
    isoItem isoLi8={"Li8",0,1.21  ,0.511,5.0 ,1.e4,0,5.e6,601};
    isoItem isoB8 ={"B8" ,0,1.11  ,0.511,5.0 ,1.e4,0,5.e6,909};
    isoItem isoC9 ={"C9" ,0,0.1825,0.051,0.5 ,1.e4,0,5.e6,800};
    isoItem isoLi9={"Li9",0,0.1717,0.081,0.8 ,1.e4,0,5.e6,415};
    isoItem isoHe8={"He8",0,0.2572,0.111,0.8 ,1.e4,0,5.e6,432};
    isoItem isoBkg={"Bkg",1,0.5   ,0.   ,50. ,1.e4,0,5.e6,416};
void prepareInf( string dataVer,string site,string fitmode,bool doSimFit)
{
    //fit information
    xt=new RooRealVar("xt","x for time fit",0,1.e6);//should be big=1.e6!needed to calculate integral of pdf = coe
    xe=new RooRealVar("xe","x for spec fit",0,1.e6);//should be big=1.e6!needed to calculate integral of pdf = coe
    rateMu=new RooRealVar("rateMu","rateMu",0);//should be 0!needed to calculate integral of pdf = coe
    //isotopes information
    iso.insert(map<string,isoItem*>::value_type("B12",&isoB12));
    iso.insert(map<string,isoItem*>::value_type("N12",&isoN12));
    iso.insert(map<string,isoItem*>::value_type("Li8",&isoLi8));
    iso.insert(map<string,isoItem*>::value_type("B8" ,&isoB8));
    iso.insert(map<string,isoItem*>::value_type("C9" ,&isoC9));
    iso.insert(map<string,isoItem*>::value_type("Li9",&isoLi9));
    iso.insert(map<string,isoItem*>::value_type("He8",&isoHe8));
    iso.insert(map<string,isoItem*>::value_type("Bkg",&isoBkg));
    std::cout<<"done iso "<<endl;
    fitInf fitB12={"B12",0,1,doSimFit,4.0 ,20.0,0.001,0.501,{"B12","N12","C9","He8","Li9","Li8","B8","Bkg"},"N12"};
    fitInf fitN12={"N12",0,1,doSimFit,14.0,20.0,0.001,0.501,{"N12","C9","He8","Li9","Bkg"},"C9"};
    fitInf fitLi8={"Li8",0,1,doSimFit,4.0 ,20.0,0.8  ,10.  ,{"Li8","B8","C9","Bkg"},"B8"};
    fitInf fitC9 ={"C9" ,0,1,doSimFit,12.0,20.0,0.15 ,2.0  ,{"C9" ,"B8","Li8","Bkg"},"Li8"};
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
    for( int i=0 ; i<10 ; i++ )
    {
        if( fit[fitmode].com[i]!="" )
        {
            genIsoPdf(iso[fit[fitmode].com[i]],fit[fitmode].xlow,fit[fitmode].xhigh,fit[fitmode].elow,fit[fitmode].ehigh); 
        }
    }

    xt->setRange(fit[fitmode].xlow,fit[fitmode].xhigh);
    std::cout<<"done xt setRange "<<endl;
    xe->setRange(fit[fitmode].elow,fit[fitmode].ehigh);
    std::cout<<"done xe setRange "<<endl;
    genFitPdf(fit[fitmode]);genData(fit[fitmode],dataVer,site);
}

int doFit(int siteNum,string dataVer,string fitMode,bool doSimFit,double fitLowRange,double fitHighRange)
{

    //===>initialize variable
    bool anaIso=1;//N12/B12 bg
    //bool anaE=0;
    int ADNumOfSite[3]={0};
    int daqHistNum=5;
    if( dataVer.find("13")!=string::npos || dataVer.find("14")!=string::npos || dataVer.find("15")!=string::npos || dataVer.find("16")!=string::npos)
    {
        ADNumOfSite[0]=2;
        ADNumOfSite[1]=2;
        ADNumOfSite[2]=4;
        daqHistNum=5;
    } else
    {
        if( dataVer.find("11")!=string::npos || dataVer.find("12")!=string::npos )
        {
            ADNumOfSite[0]=2;
            ADNumOfSite[1]=1;
            ADNumOfSite[2]=3;
            daqHistNum=4;
        }
    }
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

    //===>initialize histograms
    //livetime
    string runnum;
    TH1F* h[5];//the fifth is for daqtime
    Double_t totalTime[5]={0.};

    //B12 N12
    double tlivetime=0.;
    TH1F* showermuonNum[6]; 
    //TH1F* time2lastshowermuon[6];
    //TH1F* B12Result[5];
    //TH1F* hh[6];

    //===>get histograms from .root file for analyse
    string filename;
    filename.assign(dataVer);
    filename+="/";
    filename+=site;
    filename+="totalHisto_";
    filename+=dataVer;
    filename+=".root";
    TFile* f=new TFile(filename.c_str());

    //livetime
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
            //return true;
            continue;
        }
    }

    //B12 N12
    if(anaIso)
    {
        for( int i=0 ; i<6 ; i++ )
        {
            TString hnameLi;
            hnameLi="lidj/showermuonNumNoRed";
            //hnameLi="lidj/showermuonNum";
            hnameLi+=i+1;
            showermuonNum[i]=(TH1F*)f->Get(hnameLi);
        }
    }

    //===>calculate livetime and muonrate 
    TAxis *xaxis = h[0]->GetXaxis();
    int binNum = xaxis->GetNbins();
    //livetime
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
        }
        if( site=="EH3" )
        {
            for(int j=1  ; j<=BinBound ; j++ )
            {
                h[3]->SetBinContent(j,0);
                daqtDel+=h[4]->GetBinContent(j);
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
    //muon rate
    double NumMuon[6]={0.};
    double RateMuon[6]={0.};
    for( int j=0 ; j<5 ; j++ )
    {
        for( int jbin=0 ; jbin<binNum ; jbin++ )
        {
            NumMuon[j]+=showermuonNum[j]->GetBinContent(jbin);
        }
        RateMuon[j]=NumMuon[j]/totalDaqtime;
        RateMuon[5]+=RateMuon[j];
    }

    //===> fit iso
    std::cout<<"begin to analyse isotopes "<<endl;
    prepareInf(dataVer,site,fitMode,doSimFit);   
    if( fitLowRange!=0 && fitHighRange!=0 )
    {
        fit[fitMode].xlow=fitLowRange;
        fit[fitMode].xhigh=fitHighRange;
    }
    std::cout<<"xt range  : "<<fit[fitMode].xlow<<" ~ "<<fit[fitMode].xhigh <<endl;
    //return 1;
    if(!fit[fitMode].doSimulFit)
    {
        bool draw6slices=0;
        TString xTitle[6]={"20~60MeV","60~500MeV","0.5~1.5GeV","1.5~2.5GeV",">2.5GeV","all muon"};
        //double showerTh[6] = {0.02, 0.06, 0.5, 1.5, 2.5, 5.0};
        //B12Result[0]= new TH1F("B12Yield", "All", 5, showerTh);
        //B12Result[1]= new TH1F("B12YieldAD1", "AD1", 5, showerTh);
        //B12Result[2]= new TH1F("B12YieldAD2", "AD2", 5, showerTh);
        //B12Result[3]= new TH1F("B12YieldAD3", "AD3", 5, showerTh);
        //B12Result[4]= new TH1F("B12YieldAD4", "AD4", 5, showerTh);


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
            for( int j=0 ; j<6 ; j++ )
            {
                std::cout<<"now is  : "<<j+1<<endl;
                rateMu->setVal(-RateMuon[j]);
                //Num_tot[j]=hh[j]->Integral(1,hh[j]->FindBin(fit[fitMode].xhigh));
                if( fit[fitMode].isbinned  )
                {
                    fit[fitMode].timeFitPdf->fitTo(*(fit[fitMode].binnedData[j]),Save(),PrintLevel(-1));
                    RooAbsReal* nll = fit[fitMode].timeFitPdf->createNLL(*(fit[fitMode].binnedData[j])) ;
                    RooMinuit m(*nll) ; 
                    m.migrad() ; 
                    m.hesse() ;
                    m.contour(*(iso[fit[fitMode].con]->tFitNum),*(iso[fitMode]->tFitNum),1,2,3) ; 
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

                for( map<string,isoItem*>::iterator it=fit[fitMode].comMap.begin() ; it!=fit[fitMode].comMap.end() ; it++ )
                {
                    it->second->tNumInSlice[j]=it->second->tFitNum->getVal(0); 
                    it->second->tNumErrInSlice[j]=it->second->tFitNum->getError(); 
                    it->second->tauInSlice[j]=it->second->fitTau->getVal(0);
                    it->second->tauErrInSlice[j]=it->second->fitTau->getError();
                    if( j!=5)
                    {
                        it->second->tNum+=it->second->tFitNum->getVal(0);
                        it->second->tNumErr+=it->second->tFitNum->getError()*it->second->tFitNum->getError();
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
                    for(map<string,isoItem*>::iterator it=fit[fitMode].comMap.begin();it!=fit[fitMode].comMap.end() ; it++)
                    {
                        fit[fitMode].timeFitPdf->plotOn(mesframe,Components(*(it->second->fitExpPdf)),Name(Form("%s",it->second->isoName.c_str())),LineStyle(kDashed),LineColor(it->second->linecolor)) ;
                        leg->AddEntry(mesframe->findObject(Form("%s",it->second->isoName.c_str())),Form("%-3s ~ %0.2f",it->second->isoName.c_str(),it->second->tNumInSlice[j]),"l");
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

            if( ihist==0 )
            {
                for( int i=0 ; i<ADNumOfSite[siteNum-1] ; i++ )
                {
                    tlivetime+=totalTime[i];
                }
            }else
            {
                tlivetime=totalTime[ihist-1];
            }
            std::cout<<" "<<endl;
            std::cout<<"livetime  : "<<tlivetime/(24*3600)<<endl;

            std::cout<<" Slice |";
            //for(  map<string,isoItem*>::iterator it=fit[fitMode].comMap.begin() ; it!=fit[fitMode].comMap.end() ; it++ )
            for( int k=0;k<(int)(fit[fitMode].comMap.size());k++) 
            {
                std::cout<<Form("    %-6s |",iso[fit[fitMode].com[k]]->isoName.c_str());
            }
            std::cout<<" MuonRate |";
            if( fit[fitMode].isbinned )
            {
                std::cout<<"    chi/ndf ";
            }
            std::cout<<endl;

            for( int j=0 ; j<6 ; j++ )
            {
                std::cout<<Form(" %5i |",j);//"│" | | ｜｜
                for( int k=0;k<(int)(fit[fitMode].comMap.size());k++) 
                {
                    std::cout<<Form(" %9.1f |",fit[fitMode].comMap[fit[fitMode].com[k]]->tNumInSlice[j]);//can't use "iso[fit[fitMode].com[k]]",not is reference type.
                }
                std::cout<<Form(" %8.5f |",RateMuon[j]);
                if( fit[fitMode].isbinned )
                {
                    std::cout<<Form(" %8.2f/%i= %5.2f",fit[fitMode].chi[j],fit[fitMode].ndf,fit[fitMode].chi[j]/fit[fitMode].ndf); 
                }
                std::cout<<endl;

            }
            std::cout<<"      |       Total number      |     Rate(/day/AD) "<<endl;
            for( int k=0;k<(int)(fit[fitMode].comMap.size());k++) 
            {
                fit[fitMode].comMap[fit[fitMode].com[k]]->rate=fit[fitMode].comMap[fit[fitMode].com[k]]->tNum/(tlivetime/(24*3600));
                fit[fitMode].comMap[fit[fitMode].com[k]]->rateErr=sqrt(fit[fitMode].comMap[fit[fitMode].com[k]]->tNumErr)/(tlivetime/(24*3600));
                std::cout<<Form(" %-3s  | %10.1f +- %8.1f  | %7.1f +- %7.1f",fit[fitMode].comMap[fit[fitMode].com[k]]->isoName.c_str(),fit[fitMode].comMap[fit[fitMode].com[k]]->tNum,sqrt(fit[fitMode].comMap[fit[fitMode].com[k]]->tNumErr),fit[fitMode].comMap[fit[fitMode].com[k]]->rate,fit[fitMode].comMap[fit[fitMode].com[k]]->rateErr)<<endl;
            }

            for( int j=0 ; j<6 ; j++ )
            {
                std::cout<<"muon rate  : "<<RateMuon[j]<<endl;
            }

        }

    }else
    {
        //do simultaneous fit
        for( int j=0 ; j<6 ; j++ )
        {
            RooCategory sample("sample","sample") ;
            sample.defineType("time");
            sample.defineType("spec");
            //map<string,RooDataHist*> mapping;
            //mapping["time"] = (fit[fitMode].binnedData[j]);
            //mapping["spec"] = (fit[fitMode].binnedData[j+6]);
            RooSimultaneous simPdf("simPdf","simultaneous pdf",sample) ;
            simPdf.addPdf(*(fit[fitMode].timeFitPdf),"time") ;
            simPdf.addPdf(*(fit[fitMode].specFitPdf),"spec") ;
            //RooDataHist combData("combData","combined data",RooArgList(*xt,*xe),sample,mapping);
            RooDataHist combData("combData","combined data",RooArgSet(*xt,*xe),Index(sample),Import("time",*(fit[fitMode].binnedData[j])),Import("spec",*(fit[fitMode].binnedData[j+6]))) ;
            simPdf.fitTo(combData) ;
            std::cout<<"begin to get value "<<endl;
            std::cout<<"comMap.size  : "<<fit[fitMode].comMap.size()<<endl;
            for( map<string,isoItem*>::iterator it=fit[fitMode].comMap.begin() ; it!=fit[fitMode].comMap.end() ; it++ )
            {
                it->second->tNumInSlice[j]=it->second->tFitNum->getVal(0); 
                //it->second->tNumInSlice[j];
                ////it->second->tFitNum->getVal(0); 
                ////(*it->second->tFitNum)->getVal(0); 
                //std::cout<<"it->first  : "<<it->first<<endl;
                //std::cout<<"iso[fitMode]  : "<<iso[fitMode]<<endl;
                //std::cout<<"it->second  : "<<it->second<<endl;
                //std::cout<<"it->second->isoName: "<<&it->second->isoName<<endl;
                //std::cout<<"it->second->tFitNum  : "<<&(it->second->tFitNum)<<endl;
                ////std::cout<<"it->second->isoName: "<<it->second->isoName<<endl;
                ////std::cout<<"it->second->tFitNum  : "<<*(it->second->tFitNum)<<endl;
                it->second->tNumErrInSlice[j]=it->second->tFitNum->getError(); 
                it->second->tauInSlice[j]=it->second->fitTau->getVal(0);
                it->second->tauErrInSlice[j]=it->second->fitTau->getError();
                //it->second->eNumInSlice[j]=it->second->eFitNum->getVal(0); 
                //it->second->eNumErrInSlice[j]=it->second->eFitNum->getError(); 
                if( j!=5)
                {
                    it->second->tNum+=it->second->tFitNum->getVal(0);
                    it->second->tNumErr+=it->second->tFitNum->getError()*it->second->tFitNum->getError();
                    //it->second->eNum+=it->second->eFitNum->getVal(0);
                    //it->second->eNumErr+=it->second->eFitNum->getError()*it->second->eFitNum->getError();
                }
            }
            std::cout<<"begin to plot simultaneous fit "<<endl;
            // Plot all data tagged as time sample
            RooPlot* frame1 = xt->frame(Title("Time fit")) ;
            fit[fitMode].binnedData[j]->plotOn(frame1,LineColor(kRed));
            combData.plotOn(frame1,Cut("sample==sample::time")) ;
            simPdf.plotOn(frame1,Slice(sample,"time"),ProjWData(sample,combData)) ;
            TLegend* leg1 = new TLegend(0.3,0.6,0.95,0.94);
            leg1->SetTextSize(0.05);
            for(map<string,isoItem*>::iterator it=fit[fitMode].comMap.begin();it!=fit[fitMode].comMap.end() ; it++)
            {
                simPdf.plotOn(frame1,Slice(sample,"time"),Components(*(it->second->fitExpPdf)),ProjWData(sample,combData),Name(Form("%s",it->second->isoName.c_str())),LineStyle(kDashed),LineColor(it->second->linecolor)) ;
                leg1->AddEntry(frame1->findObject(Form("%s",it->second->isoName.c_str())),Form("%-3s ~ %0.2f",it->second->isoName.c_str(),it->second->tNumInSlice[j]),"l");
            }
            leg1->SetFillColor(10);
            //gPad->SetLogy();
            // The same plot for the spec sample slice
            //RooPlot* frame2 = xe->frame(Bins(30),Title("Spectrum fit")) ;
            RooPlot* frame2 = xe->frame(Title("Spectrum fit")) ;
            fit[fitMode].binnedData[j+6]->plotOn(frame2,LineColor(kRed));
            combData.plotOn(frame2,Cut("sample==sample::spec")) ;
            simPdf.plotOn(frame2,Slice(sample,"spec"),ProjWData(sample,combData)) ;
            TLegend* leg2 = new TLegend(0.3,0.6,0.95,0.94);
            leg2->SetTextSize(0.05);
            for(map<string,isoItem*>::iterator it=fit[fitMode].comMap.begin();it!=fit[fitMode].comMap.end() ; it++)
            {
                if( it->second->isoName!="Bkg" )
                {
                    simPdf.plotOn(frame2,Slice(sample,"spec"),Components(*(it->second->fitHistPdf)),ProjWData(sample,combData),Name(Form("%sSpec",it->second->isoName.c_str())),LineStyle(kDashed),LineColor(it->second->linecolor)) ;
                    //leg2->AddEntry(frame1->findObject(Form("%s",it->second->isoName.c_str())),Form("%-3s ~ %0.2f",it->second->isoName.c_str(),it->second->eNumInSlice[j]),"l");
                    leg2->AddEntry(frame2->findObject(Form("%sSpec",it->second->isoName.c_str())),Form("%-3s ~ %0.2f",it->second->isoName.c_str(),it->second->tNumInSlice[j]),"l");
                }
            }
            leg2->SetFillColor(10);
            nameStr=Form("%sSimultaneousFit%sSlice%i",fitMode.c_str(),fit[fitMode].ifRed.c_str(),j+1);
            TCanvas* c = new TCanvas(nameStr,nameStr,800,400) ;
            c->Divide(2) ;
            c->cd(1) ; gPad->SetLeftMargin(0.15) ; frame1->GetYaxis()->SetTitleOffset(1.4) ; frame1->Draw() ;leg1->Draw();
            c->cd(2) ; gPad->SetLeftMargin(0.15) ; frame2->GetYaxis()->SetTitleOffset(1.4) ; frame2->Draw() ;leg2->Draw();
            nameStr2=Form("%s/simFitEps/%sSimultaneousFit%sSlice%i.eps",dataVer.c_str(),fitMode.c_str(),fit[fitMode].ifRed.c_str(),j+1);
            c->SaveAs(nameStr2);
            c->Draw();
            //if(j==4) c->Draw();
            //delete frame1;
            //delete leg1;
            //delete frame2;
            //delete leg2;
            //delete c;

        }
    }

    //===>print live time result
    std::cout<<""<<endl;
    std::cout<<site <<"'s infomation : "<<endl;
    std::cout<<""<<endl;
    for( int i=0 ; i<ADNumOfSite[siteNum-1] ; i++ )
    {
        std::cout<<"Total AD"<<i+1<<"LiveTime                         : "<<totalTime[i]/(24*3600)<<endl;
    }
    std::cout<<"Total DaqTime : "<<totalTime[4]/(24*3600)<<" day" <<endl;
    std::cout<<""<<endl;

    //===>write into .root file
    //string rootname=site;
    //rootname+="FitResult_"+dataVer+".root";
    //TFile* file = new TFile(rootname.c_str(),"RECREATE");
    //file->cd();
    //for( int i=0 ; i<5 ; i++ )
    //{
    //h[i]->Write();
    //}
    //if( anaIso )
    //{
    //for( int i=0 ; i<5 ; i++ )
    //{
    //hh[i]->Write();
    //}
    ////B12Result[0]->Write();
    //}
    //file->Close();
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
        runlistName.assign(verSuf,1,verSuf.size()-1);
        runlistName+="/";
        runlistName+=runlistSiteNum[i];
        runlistName+="totalHisto";
        runlistName+=verSuf;
        //runlistName+=*it;
        runlistName+=".root";
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

    int IsoNumFit(string dataVer,int siteNum=0,string FitMode="B12",bool doSimFit=1,double FitLowRange=0.,double FitHighRange=0.)
    //int main(int argc, char *argv[])
{
    //string dataVer;
    //int siteNum=0;
    //string FitMode;
    //double FitLowRange=0.;
    //double FitHighRange=0.;
    //bool doSimFit=0;
    //std::cout<<"argc  : "<<argc<<endl;
    //dataVer=argv[1];
    //std::cout<<"dataVer  : "<<dataVer<<endl;
    //siteNum=atoi(argv[2]);
    //std::cout<<"siteNum  : "<<siteNum<<endl;
    //FitMode=argv[3];
    //std::cout<<"FitMode  : "<<FitMode<<endl;
    //doSimFit=atoi(argv[4]);
    //std::cout<<"doSimFit  : "<<doSimFit<<endl;
    //if( argc==7 )
    //{
    //FitLowRange=atof(argv[5]);
    //std::cout<<"FitLowRange  : "<<FitLowRange<<endl;
    //FitHighRange=atof(argv[6]);
    //std::cout<<"FitHighRange  : "<<FitHighRange<<endl;
    //}
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
            doFit(i,dataVer,FitMode,doSimFit,FitLowRange,FitHighRange);
            std::cout<<" "<<endl;
            std::cout<<" "<<endl;
            std::cout<<" "<<endl;
            std::cout<<" "<<endl;
        }

    }else if(siteNum>=1 && siteNum<=3)
    {
        std::cout<<"====> begin to analyse EH"<<siteNum<<"'s DaqTime,N12/B12 "<<endl;
        std::cout<<"dataVersion  : "<<dataVer<<endl;
        doFit(siteNum,dataVer,FitMode,doSimFit,FitLowRange,FitHighRange);
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

