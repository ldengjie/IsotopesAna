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
#include  "RooAddition.h"
using namespace std;
using namespace RooFit;
int main(int argc, char *argv[])
{
    //gROOT->ProcessLine("using namespace RooFit;");
    RooRealVar* xt=new RooRealVar("xt","x for spec fit",0,1.e6);//should be big=1.e6!needed to calculate integral of pdf = coe
    RooRealVar* xe=new RooRealVar("xe","x for spec fit",0,1.e6);//should be big=1.e6!needed to calculate integral of pdf = coe
    TString nameStr;
    string isoname="B12";

    RooRealVar* rateMu=new RooRealVar("rateMu","rateMu",-0.0629969);

    RooRealVar* B12Tau= new RooRealVar("B12Tau", "B12Tau", -0.0291,"s");
    RooRealVar* B12FitNum=new RooRealVar("B12FitNum","B12FitNum",159061.3,100,6000000);
    RooFormulaVar* B12Lambda=new RooFormulaVar("B12Lambda","B12Lambda","1/@0 + @1",RooArgList(*B12Tau, *rateMu));
    RooExponential* B12ExpPdf=new RooExponential("B12ExpPdf","B12ExpPdf", *xt,*B12Lambda);
    RooExtendPdf* B12ExtendPdf=new RooExtendPdf("B12ExtendPdf","B12ExtendPdf",*B12ExpPdf,*B12FitNum) ;

    RooRealVar* BkgTau= new RooRealVar("BkgTau", "BkgTau", -0.5,"s");
    RooRealVar* BkgFitNum=new RooRealVar("BkgFitNum","BkgFitNum",66002,100,6000000);
    RooFormulaVar* BkgLambda=new RooFormulaVar("BkgLambda","BkgLambda","@0 + @1",RooArgList(*BkgTau, *rateMu));
    RooExponential* BkgExpPdf=new RooExponential("BkgExpPdf","BkgExpPdf", *xt,*BkgLambda);
    RooExtendPdf* BkgExtendPdf=new RooExtendPdf("BkgExtendPdf","BkgExtendPdf",*BkgExpPdf,*BkgFitNum) ;

    RooArgList timeFitComList;
    timeFitComList.add(*B12ExtendPdf);
    timeFitComList.add(*BkgExtendPdf);
    RooAddPdf* timeFitPdf=new RooAddPdf("timeModel","timeModel",timeFitComList) ;

    //RooRealVar* xe=new RooRealVar("xe","x for spec fit",0,1.e6);
    xt->setRange("tRange",0.001,0.501);
    RooAbsReal* tTmpCoe =B12ExpPdf->createIntegral(*xt,NormSet(*xt),Range("tRange"));
    std::cout<<"tTmpCoe  : "<<tTmpCoe->getVal()<<endl;
    RooRealVar* tCutCoe=new RooRealVar("tCutCoe","tCutCoe",tTmpCoe->getVal());
    TFile* f=new TFile("IsoTheoreticalSpec.root","read");
            nameStr=Form("%sspecHistogramVsp.d.f",isoname.c_str());
            TCanvas* ce2 = new TCanvas(nameStr,nameStr,1200,400) ;
            ce2->Divide(3) ;
    nameStr=Form("%sSpectraAfterCor",isoname.c_str());
    TH1F* h=(TH1F*)f->Get(nameStr);
    h->Rebin(250);
            ce2->cd(1) ;  h->Draw() ;
    RooDataHist* ehd=new RooDataHist("hd","hd",*xe,h);
            RooPlot* framee = xe->frame(Title("spec histogram")) ;
            ehd->plotOn(framee,LineColor(kRed),DataError(RooAbsData::None));
            ce2->cd(2) ;  framee->Draw() ;
    std::cout<<"ehd->numEntries()  : "<<ehd->numEntries()<<endl;
    nameStr=Form("%shpdf",isoname.c_str());
    RooHistPdf* fitHistPdf=new RooHistPdf(nameStr,nameStr,*xe,*ehd,2) ;
            RooPlot* framee1 = xe->frame(Title("spec p.d.f")) ;
            fitHistPdf->plotOn(framee1),LineColor(kBlue) ;
            ce2->cd(3) ;  framee1->Draw() ;
            nameStr=Form("P14A/dataEps/test%sspecHistogramVsp.d.f.eps",isoname.c_str());
            ce2->SaveAs(nameStr);
    xe->setRange("eRange",5.0,20.0);
    RooAbsReal* eTmpCoe =fitHistPdf->createIntegral(*xe,NormSet(*xe),Range("eRange"));
    RooRealVar* eCutCoe=new RooRealVar("eCutCoe","eCutCoe",eTmpCoe->getVal());
    //RooFormulaVar* eFitNum=new RooFormulaVar("eFitNum","eFitNum","@0/@1*@2",RooArgList(*B12FitNum,*tCutCoe,*eCutCoe));
    RooRealVar* eFitNum=new RooRealVar("eFitNum","eFitNum",159061.3,100,6000000);
    RooExtendPdf* hdExtendPdf=new RooExtendPdf("hdExtendPdf","hdExtendPdf",*fitHistPdf,*eFitNum) ;
    RooArgList specFitComList;
    specFitComList.add(*hdExtendPdf);
    RooAddPdf* specFitPdf=new RooAddPdf("specMode","specMode",specFitComList);


    /*
       TFile* f=new TFile("IsoTheoreticalSpec.root","read");
       nameStr=Form("%sSpectraAfterCor",isoname.c_str());
       TH1F* h=(TH1F*)f->Get(nameStr);
    //h->Scale(10000000);
    ce->cd(1) ;  h->Draw() ;
    RooDataHist* hd =new RooDataHist("hd","hd",*xe,h);
    RooPlot* framee = xe->frame() ;
    hd->plotOn(framee,DataError(RooAbsData::None),MarkerSize(0.1));
    ce->cd(2) ;  framee->Draw() ;
    */
    /*
       TH1F* hh=new TH1F("hh","hh",100,1,10);
       for( int i=1 ; i<=100 ; i++ )
       {
    //hh->SetBinContent(i,i);
    hh->SetBinContent(i,(-(i/10.-5)*(i/10.-5)+25)/100000.);
    }
    ce->cd(3);
    hh->Draw();
    RooDataHist* hd2 =new RooDataHist("hd2","hd2",*xe,hh);
    RooPlot* frame2 = xe->frame() ;
    hd2->plotOn(frame2,DataError(RooAbsData::None));
    ce->cd(4) ;  frame2->Draw() ;
    */
    //nameStr=Form("P14A/EH1iso_P14A.root",dataVer.c_str(),site.c_str(),dataVer.c_str());

    nameStr=Form("%sspecHistogramVsp.d.f",isoname.c_str());
    TCanvas* ce = new TCanvas(nameStr,nameStr,1200,800) ;
    ce->Divide(3,2) ;

    TFile* f2=new TFile("P14A/EH1iso_P14A.root","read");

    TH1F* h4=(TH1F*)f2->Get("time2lastshowermuonNoRed4_5.0_20.0");
    int hmax=h4->FindBin(0.501);
    std::cout<<"hman  : "<<hmax<<endl;
    int hmin=h4->FindBin(0.001);
    std::cout<<"hmin  : "<<hmin<<endl;
    int hBinNum=hmax-hmin;
    std::cout<<"hBinNum  : "<<hBinNum<<endl;
    TH1F* ht=new TH1F("slice4","slice4",hBinNum,0.001,0.501);
    for( int j=1 ; j<=hBinNum ; j++ )
    {
        ht->SetBinContent(j,h4->GetBinContent(hmin+j-1));
    }
    //ht->Rebin(8);
    ce->cd(1) ;  ht->Draw() ;
    RooDataHist* hd1=new RooDataHist(Form("%stime2lastmuonBinned",isoname.c_str()),"time2lastmuon binned data",*xt,ht);
    RooPlot* frame1 = xt->frame() ;
    hd1->plotOn(frame1,DataError(RooAbsData::None),MarkerSize(0.1));
    ce->cd(2) ;  frame1->Draw() ;

    nameStr=Form("%sSpecNoRedSlice4_5.0_20.0",isoname.c_str());
    TH1F* hs=(TH1F*)f2->Get(nameStr);
    ce->cd(3);
    hs->Draw();
    xe->setRange(5,20);

    RooDataHist* hd3=new RooDataHist(Form("%sspecBinned",isoname.c_str()),"spec binned data",*xe,hs);
    RooPlot* frame3 = xe->frame() ;
    hd3->plotOn(frame3,DataError(RooAbsData::None),MarkerSize(0.1));
    ce->cd(4) ;  frame3->Draw() ;

    RooCategory sample("sample","sample") ;
    sample.defineType("time");
    sample.defineType("spec");
    //std::map<string,TH1F*> histMap;
    //histMap.insert(pair<string,TH1F*>("time",ht));
    //histMap.insert(pair<string,TH1F*>("spec",hs));
    std::cout<<"hd1->numEntries()  : "<<hd1->numEntries()<<endl;
    std::cout<<"hd3->numEntries()  : "<<hd3->numEntries()<<endl;
    xt->setBins(hd1->numEntries());
    xe->setBins(hd3->numEntries());
    //RooDataHist combData("combData","combined data",RooArgSet(*xt,*xe),Index(sample),Import("time",*(hd1)),Import("spec",*(hd3))) ;
    //RooDataHist combData("combData","combined data",RooArgSet(*xt,*xe),Index(sample),Import("time",*(ht)),Import("spec",*(hs))) ;
    //map<int,string> mabb;
    //RooDataHist combData("combData","combined data",RooArgSet(*xt,*xe),Index(sample),histMap,1) ;


    //nameStr=Form("P14A/dataEps/%sspecHistogramVsp.d.f.eps",isoname.c_str());
    //ce->SaveAs(nameStr);


    //RooRealVar* xt=new RooRealVar("xt","x for time fit",0,1.e6);

    //RooSimultaneous simPdf("simPdf","simultaneous pdf",sample) ;
    //simPdf.addPdf(*specFitPdf,"spec") ;
    //simPdf.addPdf(*timeFitPdf,"time") ;
    //simPdf.fitTo(combData,SumW2Error(kTRUE),PrintEvalErrors(10)) ;

                    RooAbsReal* nll1 = timeFitPdf->createNLL(*(hd1)) ;
                    //RooAbsReal* nll2 = specFitPdf->createNLL(*(hd3)) ;
                    RooAbsReal* nll2 = hdExtendPdf->createNLL(*(hd3)) ;
                    RooAddition nll("nll","nll",RooArgSet(*nll1,*nll2)) ;

                    RooMinuit m1(*nll1) ; 
                    m1.migrad() ; 
                    m1.hesse() ;
                    RooFitResult* r1 = m1.save() ;
                    std::cout<<"!!!cout m1"<<endl;
                    r1->Print("v") ;
                    std::cout<<"!!!end m1"<<endl;

                    RooMinuit m2(*nll2) ; 
                    m2.migrad() ; 
                    m2.hesse() ;
                    RooFitResult* r2 = m2.save() ;
                    std::cout<<"!!!cout m2"<<endl;
                    r2->Print("v") ;
                    std::cout<<"!!!end m2"<<endl;

                    //RooMinuit m(nll) ; 
                    //m.migrad() ; 
                    //m.hesse() ;
                    //RooFitResult* r = m.save() ;
                    //r->Print("v") ;
    RooPlot* frame4 = xt->frame(Bins(500),Title("Time fit")) ;
    //combData.plotOn(frame4,Cut("sample==sample::time"),DataError(RooAbsData::None),MarkerSize(0.1)) ;
    //simPdf.plotOn(frame4,Slice(sample,"time"),ProjWData(sample,combData)) ;
    //simPdf.plotOn(frame4,Slice(sample,"time"),Components(*(B12ExpPdf)),ProjWData(sample,combData),Name(Form("%s",isoname.c_str())),LineStyle(kDashed),LineColor(kRed)) ;
    //simPdf.plotOn(frame4,Slice(sample,"time"),Components(*(BkgExpPdf)),ProjWData(sample,combData),Name(Form("%s",isoname.c_str())),LineStyle(kDashed),LineColor(kBlue)) ;
    ce->cd(5) ;  frame4->Draw() ;
    RooPlot* frame5 = xe->frame(Bins(60),Title("Spectrum fit")) ;
    //combData.plotOn(frame5,Cut("sample==sample::spec"),DataError(RooAbsData::None),MarkerSize(0.1)) ;
    //simPdf.plotOn(frame5,Slice(sample,"spec"),ProjWData(sample,combData)) ;
    ce->cd(6) ;  frame5->Draw() ;
    nameStr=Form("P14A/dataEps/%stestRooDataHist.eps",isoname.c_str());
    ce->SaveAs(nameStr);
    std::cout<<"all done "<<endl;
    return 1;
}
