#include  <iostream>
#include  <TCanvas.h>
#include  <TH1F.h>
#include  <TFile.h>
#include "RooRealVar.h"
#include  "RooDataSet.h"
#include  "RooAbsReal.h"
#include "RooExponential.h"
#include "RooMinuit.h"
#include "RooAddPdf.h"
#include "RooPlot.h"
#include "RooFitResult.h"
#include  <string>
#include  <vector>
#include "RooHistPdf.h"
#include "RooExtendPdf.h"
#include "RooDataHist.h"
using namespace std;
using namespace RooFit;
    int main(int argc, char *argv[])
//void testRooHistPdf()
{
    RooRealVar* xe=new RooRealVar("xe","x for spec fit",0,1.e6);//should be big=1.e6!needed to calculate integral of pdf = coe
    TString nameStr;
    string isoname="B12";

    TFile* f=new TFile("IsoTheoreticalSpec.root","read");
            nameStr=Form("%sspecHistogramVsp.d.f",isoname.c_str());
            TCanvas* ce2 = new TCanvas(nameStr,nameStr,1200,800) ;
            ce2->Divide(3,2) ;
            nameStr=Form("%sSpectraAfterCor",isoname.c_str());
    TH1F* h0=(TH1F*)f->Get(nameStr);
            int hemax=h0->FindBin(20.);
            int hemin=h0->FindBin(5.);
            int heBinNum=hemax-hemin;
            std::cout<<"heBinNum  : "<<heBinNum<<endl;
            nameStr=Form("slice%ispec",4);
            TH1F* h=new TH1F(nameStr,nameStr,heBinNum,5.,20.);
            for( int j=1 ; j<=heBinNum ; j++ )
            {
                h->SetBinContent(j,h0->GetBinContent(hemin+j-1));
            }
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
            fitHistPdf->plotOn(framee1,LineColor(kBlue) );
            ce2->cd(3) ;  framee1->Draw() ;
            //xe->setRange("eRange",5.0,20.0);
    RooRealVar* eFitNum=new RooRealVar("eFitNum","eFitNum",159061.3,100,6000000);
    RooExtendPdf* hdExtendPdf=new RooExtendPdf("hdExtendPdf","hdExtendPdf",*fitHistPdf,*eFitNum) ;
    for( int i=1 ; i<10000 ; i++ )
    {
        if(hdExtendPdf->getVal(*xe=5+i*(20.-5.)/1000)==0  )
        {
            std::cout<<"Find zero  : "<<5+i*(20.-5.)/1000<<endl;
            std::cout<<"zero is at  : "<<h->FindBin(5+i*(20.-5.)/1000)<<endl;
            break;
        }
        
    }
    

    TFile* f2=new TFile("P14A/EH1iso_P14A.root","read");
            nameStr=Form("%sSpecNoRedSlice4_5.0_20.0",isoname.c_str());
    TH1F* hs0=(TH1F*)f2->Get(nameStr);
            int hsmax=hs0->FindBin(20.);
            int hsmin=hs0->FindBin(5.);
            int hsBinNum=hsmax-hsmin;
            std::cout<<"hsBinNum  : "<<hsBinNum<<endl;
            nameStr=Form("slice%ispecdata",4);
            TH1F* hs=new TH1F(nameStr,nameStr,hsBinNum,5.,20.);
            for( int j=1 ; j<=hsBinNum ; j++ )
            {
                hs->SetBinContent(j,hs0->GetBinContent(hsmin+j-1));
            }
            int hsNum=hs->GetXaxis()->GetNbins();
            for( int i=1 ; i<=hsNum ; i++ )
            {
                if( hs->GetBinContent(i)<=0 )
                {
                    std::cout<<"bin content is <=0  : "<<i<<":"<<hs->GetBinContent(i) <<endl;
                    hs->SetBinContent(i,-hs->GetBinContent(i));
                    std::cout<<"bin content is  : "<<i<<":"<<hs->GetBinContent(i) <<endl;
                }
            }
            for( int i=1 ; i<=hsNum ; i++ )
            {
                //if( i>30 && i<=36 )
                if( i>30 && i<=36 )
                {
                    hs->SetBinContent(i,-hs->GetBinContent(i));
                    
                }
                if( i>36 )
                {
                    hs->SetBinContent(i,0);
                }
            }
    
    //ce->cd(3);
    //hs->Draw();
            xe->setRange(5,20);
    xe->setBins(60) ;
    RooDataHist* hd3=new RooDataHist(Form("%sspecBinned",isoname.c_str()),"spec binned data",*xe,hs);
            RooPlot* frame3 = xe->frame() ;
            //hd3->plotOn(frame3,DataError(RooAbsData::None),MarkerSize(0.1));
            hd3->plotOn(frame3);
            hdExtendPdf->plotOn(frame3,LineColor(kRed) );
            ce2->cd(4) ;  frame3->Draw() ;
    /*
                    RooAbsReal* nll2 = hdExtendPdf->createNLL(*(hd3)) ;
                    RooMinuit m2(*nll2) ; 
                    m2.migrad() ; 
                    m2.hesse() ;
                    RooFitResult* r2 = m2.save() ;
                    std::cout<<"!!!cout m2"<<endl;
                    r2->Print("v") ;
                    std::cout<<"!!!end m2"<<endl;
                    */
    RooDataSet* data = fitHistPdf->generate(*xe,10000) ;
    RooDataHist* hist1 = data->binnedClone() ;
            nameStr=Form("slice%ispecdataforGen",4);
            TH1F* hg=new TH1F(nameStr,nameStr,hsBinNum,5.,20.);
    for( int i=0 ; i<hist1->numEntries() ; i++ )
    {
        
            hist1->get(i);
            hg->SetBinContent(i+1,hist1->weight());
    }
    hg->Scale(1.01);
    RooDataHist* hdg=new RooDataHist(Form("%sspecBinnedformGen",isoname.c_str()),"spec binned data",*xe,hg);
            RooPlot* frame6 = xe->frame() ;
            //hd3->plotOn(frame3,DataError(RooAbsData::None),MarkerSize(0.1));
            hdg->plotOn(frame6);
            //hdg->plotOn(frame6,LineColor(kRed) );
            ce2->cd(6) ;  frame6->Draw() ;
    
                nameStr=Form("%5s   %11s   %11s","index","hs","hg");
                std::cout<<nameStr<<endl;
            for( int i=0 ; i<=hsNum+1 ; i++ )
            {
                nameStr=Form("%3i   %5.5f+-%5.5f   %5.5f+-%5.5f",i,hs->GetBinContent(i),hs->GetBinError(i),hg->GetBinContent(i),hg->GetBinError(i));
                std::cout<<nameStr<<endl;

            }

            hdExtendPdf->fitTo(*(hd3)) ;
            //hdExtendPdf->fitTo(*(hdg)) ;
    //hdExtendPdf->fitTo(*data) ;
    //hdExtendPdf->fitTo(*hist1) ;

    std::cout<<"hist1->numEntries()   : "<<hist1->numEntries() <<endl;
    std::cout<<"hist1->isWeighted()  : "<<hist1->isWeighted()<<endl;
    hist1->get(5) ;
    std::cout<<"hist1->weight(5)  : "<<hist1->weight()<<endl;
    hist1->get(24) ;
    std::cout<<"hist1->weight(24)  : "<<hist1->weight()<<endl;
    std::cout<<"hist1->binVolume(24)  : "<<hist1->binVolume()<<endl;
    std::cout<<"hd3->numEntries()   : "<<hd3->numEntries() <<endl;
    std::cout<<"hd3->isWeighted()  : "<<hd3->isWeighted()<<endl;
    hd3->get(5) ;
    std::cout<<"hd3->weight(5)  : "<<hd3->weight()<<endl;
    hd3->get(24) ;
    std::cout<<"hd3->weight(24)  : "<<hd3->weight()<<endl;
    std::cout<<"hd3->binVolume(24)  : "<<hd3->binVolume()<<endl;

    //std::cout<<"hist1->weight(25)  : "<<hist1->weight(25)<<endl;
    //std::cout<<"hist1->printValue()  : "<<hist1->printValue()<<endl;
    //fitHistPdf->fitTo(*data) ;
            RooPlot* framee2 = xe->frame(Title("spec data for p.d.f")) ;
            data->plotOn(framee2) ;
            hdExtendPdf->plotOn(framee2,LineColor(kRed) );
            ce2->cd(5) ;  framee2->Draw() ;
            nameStr=Form("P14A/dataEps/test%sspecHistogramVsp.d.f.eps",isoname.c_str());
            ce2->SaveAs(nameStr);
}
