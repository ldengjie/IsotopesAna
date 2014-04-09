#define SingleTree_cxx

#include "SingleTree.h"
#include <TH2.h>
#include <TStyle.h>
#include  <THStack.h>
#include  <iostream>

void SingleTree::Begin(TTree * /*tree*/)
{
    TString option = GetOption();
    TString IsoModeTmp=option(7,3);
    genIsoSpec=1;
    genAmCNeu=0;
    genTimeFitDis=1;
    //IsoMode="C9";//Li8,C99,B12
    IsoMode=IsoModeTmp;//Li8,C99,B12
    ifRed[1]="NoRed";
    ifRed[0]="";
    for( int i=0 ; i<16 ; i++ )
    {
        T2lastMuon[i]=0.;
    }

    if( IsoMode=="B12" )
    {
        LowEdge=4.0;
        HighEdge=20.0;
        LowEdge4e=LowEdge;
        HighEdge4e=HighEdge;
        signalWinLow=0.002;
        signalWinHigh=0.06;
        offWinLow=0.502;
        offWinHigh=0.560;
    }else if( IsoMode=="Li8" )
    {
        LowEdge=14.0;//for N12 time fit,that for Li8 has been done while B12.
        HighEdge=20.0;
        LowEdge4e=4.0;
        HighEdge4e=20.0;
        signalWinLow=0.6;
        signalWinHigh=4.0;
        offWinLow=10.6;
        offWinHigh=14.0;
    } else if( IsoMode=="C99" )
    {
        LowEdge=12.0;
        HighEdge=20.0;
        LowEdge4e=LowEdge;
        HighEdge4e=HighEdge;
        signalWinLow=0.2;
        signalWinHigh=0.6;
        offWinLow=10.2;
        offWinHigh=10.6;
    }


    TString DataVerTmp=option(3,4);
    dataVer=DataVerTmp;
    option=option(0,3);
    std::cout<<"dataVer  : "<<dataVer<<endl;
    int daqHistNum;
    if( dataVer=="P12E"||dataVer=="P12C" )
    {
        daqHistNum=4;
    } else
    {
        daqHistNum=5;
    }
    std::cout<<"option  : "<<option<<endl;
    ADNum=daqHistNum-1;
    site=2;
    if( option=="EH1")
    {
        site=0;
        ADNum=2;
    }
    if( option=="EH2" )
    {
        site=1;
        ADNum=daqHistNum-3;
    }
    option+="iso_";
    option+=dataVer;
    option+=".root";
    option=dataVer+"/"+option;
    file = new TFile(option,"update");
    if( genTimeFitDis )
    {
        for( int i=0 ; i<5 ; i++ )
        {
            histname=Form("time2lastshowermuon%i_%0.1f_%0.1f",i+1,LowEdge,HighEdge);
            time2lastshowermuon[i]=new TH1F(histname,"time2lastshowermuon",99999,0.001,100);
            histname=Form("time2lastshowermuonNoRed%i_%0.1f_%0.1f",i+1,LowEdge,HighEdge);
            time2lastshowermuonNoRed[i]=new TH1F(histname,"time2lastshowermuon",99999,0.001,100);
        }

        histname=Form("time2Allmuon_%0.1f_%0.1f",LowEdge,HighEdge);
        time2Allmuon[0]=new TH1F(histname,"time2Allmuon",99999,0.001,100);
        histname=Form("time2AllmuonNoRed_%0.1f_%0.1f",LowEdge,HighEdge);
        time2Allmuon[1]=new TH1F(histname,"time2Allmuon",99999,0.001,100);

        for( int i=0 ; i<5 ; i++ )
        {
            time2lastmuon[i]=new TTree(Form("slice%i_%0.1f_%0.1f",i+1,LowEdge,HighEdge),"time2lastmuon");
            time2lastmuonNoRed[i]=new TTree(Form("sliceNoRed%i_%0.1f_%0.1f",i+1,LowEdge,HighEdge),"time2lastmuon NoRed");

            time2lastmuon[i]->Branch("xt",&T2lastMuon[4+i],"xt/F");
            time2lastmuonNoRed[i]->Branch("xt",&T2lastMuon[10+i],"xt/F");
        }
        time2lastmuon[5]=new TTree(Form("slice6_%0.1f_%0.1f",LowEdge,HighEdge),"time2lastmuon");
        time2lastmuon[5]->Branch("xt",&minT2allmuon[0],"xt/F");
        time2lastmuonNoRed[5]=new TTree(Form("sliceNoRed6_%0.1f_%0.1f",LowEdge,HighEdge),"time2lastmuon NoRed");
        time2lastmuonNoRed[5]->Branch("xt",&minT2allmuon[1],"xt/F");
    }
    if( genAmCNeu )
    {
        for( int i=0 ; i<4 ; i++ )
        {
            histname=Form("singleUpperAD%i_%0.1f_%0.1f",i+1,LowEdge,HighEdge);
            singleUpper[i]=new TH1F(histname,"single upper",400,0,20);
            histname=Form("singleLowerAD%i_%0.1f_%0.1f",i+1,LowEdge,HighEdge);
            singleLower[i]=new TH1F(histname,"single lower",400,0,20);
            histname=Form("AmCSpecAD%i_%0.1f_%0.1f",i+1,LowEdge,HighEdge);
            AmCSpec[i]=new TH1F(histname,"AmC spectrum",400,0,20);
        }

    }

    if( genIsoSpec )
    {
        for( int j=0 ; j<2 ; j++ )
        {
            for( int i=0 ; i<6 ; i++ )
            {
                std::cout<<"6.1 "<<endl;
                sprintf(nameChar,"%ssignalEnergy%sSlice%i_%0.1f_%0.1f",IsoMode.c_str(),ifRed[j].c_str(),i+1,LowEdge,HighEdge);
                signalWin[j][i]=new TH1F(nameChar,nameChar,80,0,20);
                std::cout<<"6.2 "<<endl;
                sprintf(nameChar,"%soffEnergy%sSlice%i_%0.1f_%0.1f",IsoMode.c_str(),ifRed[j].c_str(),i+1,LowEdge,HighEdge);
                offWin[j][i]=new TH1F(nameChar,nameChar,80,0,20);
                std::cout<<"6.3 "<<endl;
                sprintf(nameChar,"%ssignalXY%sSlice%i_%0.1f_%0.1f",IsoMode.c_str(),ifRed[j].c_str(),i+1,LowEdge,HighEdge);
                std::cout<<"6.31 "<<endl;
                std::cout<<"nameChar  : "<<nameChar<<endl;
                signalWinXY[j][i]=new TH2F(nameChar,nameChar,600,-3000.,3000.,600,-3000.,3000.);
                std::cout<<"6.4 "<<endl;
                sprintf(nameChar,"%soffXY%sSlice%i_%0.1f_%0.1f",IsoMode.c_str(),ifRed[j].c_str(),i+1,LowEdge,HighEdge);
                offWinXY[j][i]=new TH2F(nameChar,nameChar,600,-3000,3000,600,-3000,3000);
                std::cout<<"6.5 "<<endl;
                sprintf(nameChar,"%ssignalRZ%sSlice%i_%0.1f_%0.1f",IsoMode.c_str(),ifRed[j].c_str(),i+1,LowEdge,HighEdge);
                signalWinRZ[j][i]=new TH2F(nameChar,nameChar,300,0,3000,600,-3000,3000);
                std::cout<<"6.6 "<<endl;
                sprintf(nameChar,"%soffRZ%sSlice%i_%0.1f_%0.1f",IsoMode.c_str(),ifRed[j].c_str(),i+1,LowEdge,HighEdge);
                offWinRZ[j][i]=new TH2F(nameChar,nameChar,300,0,3000,600,-3000,3000);
                std::cout<<"6.7 "<<endl;
                sprintf(nameChar,"%ssingleSpecVsTime%sSlice%i_%0.1f_%0.1f",IsoMode.c_str(),ifRed[j].c_str(),i+1,LowEdge,HighEdge);
                singleSpecVsTime[j][i]=new TH2F(nameChar,nameChar,100000,0,100,400,0,20);
                std::cout<<"6.8 "<<endl;
                std::cout<<" "<<endl;
            }
            //sprintf(nameChar,"%ssignalWindow2AllMuon%s_%0.1f_%0.1f",IsoMode.c_str(),ifRed[j].c_str(),LowEdge,HighEdge);
            //signalWin[j][5]=new TH1F(nameChar,nameChar,80,0,20);
            //sprintf(nameChar,"%soffWindow2AllMuon%s_%0.1f_%0.1f",IsoMode.c_str(),ifRed[j].c_str(),LowEdge,HighEdge);
            //offWin[j][5]=new TH1F(nameChar,nameChar,80,0,20);
            //sprintf(nameChar,"%ssingleSpecVsTimeAllMuon%s_%0.1f_%0.1f",IsoMode.c_str(),ifRed[j].c_str(),LowEdge,HighEdge);
            //singleSpecVsTime[j][5]=new TH2F(nameChar,nameChar,1000000,0,1000,80,0,20);
        }
    }
    std::cout<<"finished Begin() "<<endl;
}

void SingleTree::SlaveBegin(TTree * /*tree*/)
{

    TString option = GetOption();

}

Bool_t SingleTree::Process(Long64_t entry)
{
    GetEntry(entry);
    if( entry%2000000==0 )
    {
        std::cout<<entry<<endl;
    }
    //isotopes spectrum
    if( genIsoSpec )
    {
        if( energy>LowEdge4e&&energy<HighEdge4e )
        {
            for( int i=0 ; i<2 ; i++ )
            {
                minT2allmuon[i]=1000.;
                for( int j=0 ; j<5 ; j++ )
                {
                    singleSpecVsTime[i][j]->Fill(T2lastMuon[j+4+6*i],energy);
                    minT2allmuon[i]=minT2allmuon[i]<T2lastMuon[j+4+6*i]?minT2allmuon[i]:T2lastMuon[j+4+6*i];
                    if( T2lastMuon[j+4+6*i]>signalWinLow &&T2lastMuon[j+4+6*i]<signalWinHigh )
                    {
                        signalWinXY[i][j]->Fill(x,y);
                        signalWinRZ[i][j]->Fill(sqrt(x*x+y*y),z);
                        signalWin[i][j]->Fill(energy);
                    }
                    if( T2lastMuon[j+4+6*i]>offWinLow &&T2lastMuon[j+4+6*i]<offWinHigh )
                    {
                        offWinXY[i][j]->Fill(x,y);
                        offWinRZ[i][j]->Fill(sqrt(x*x+y*y),z);
                        offWin[i][j]->Fill(energy);
                    }
                }
                singleSpecVsTime[i][5]->Fill(minT2allmuon[i],energy);
                if( minT2allmuon[i]>signalWinLow &&minT2allmuon[i]<signalWinHigh )
                {
                    signalWinXY[i][5]->Fill(x,y);
                    signalWinRZ[i][5]->Fill(sqrt(x*x+y*y),z);
                    signalWin[i][5]->Fill(energy);
                }
                if( minT2allmuon[i]>offWinLow &&minT2allmuon[i]<offWinHigh )
                {
                    offWinXY[i][5]->Fill(x,y);
                    offWinRZ[i][5]->Fill(sqrt(x*x+y*y),z);
                    offWin[i][5]->Fill(energy);
                }
            }
        }
    }
    if( energy<LowEdge || energy>HighEdge)
    {
        return 1; 
    }
    //AmC netron
    if( genAmCNeu )
    {
        if( z>0 )
        {
            singleUpper[det-1]->Fill(energy);
        }
        if( z<0 )
        {
            singleLower[det-1]->Fill(energy);
        }

    }
    if( genTimeFitDis )
    {
        for( int j=0 ; j<5 ; j++ )
        {
            time2lastshowermuon[j]->Fill(T2lastMuon[j+4]);
            time2lastshowermuonNoRed[j]->Fill(T2lastMuon[j+10]);

        }
        minT2allmuon[0]=1000.;
        minT2allmuon[1]=1000.;
        for( int j=0 ; j<5 ; j++ )
        {
            minT2allmuon[0]=minT2allmuon[0]<T2lastMuon[j+4]?minT2allmuon[0]:T2lastMuon[j+4];
            minT2allmuon[1]=minT2allmuon[1]<T2lastMuon[j+10]?minT2allmuon[1]:T2lastMuon[j+10];
        }
        //if( T2lastMuon[2+10]>0 )
        //{
        //std::cout<<"find one "<<T2lastMuon[2+10]<<endl;
        //}
        time2Allmuon[0]->Fill(minT2allmuon[0]);
        time2Allmuon[1]->Fill(minT2allmuon[1]);
        for( int j=0 ; j<6 ; j++ )
        {
            time2lastmuon[j]->Fill();
            time2lastmuonNoRed[j]->Fill();
        }


    }


    return kTRUE;
}

void SingleTree::SlaveTerminate()
{

}

void SingleTree::Terminate()
{
    std::cout<<"Now is in Terminate() "<<endl;
    if( genIsoSpec )
    {
        for( int j=0 ; j<2 ; j++ )
        {
            for( int i=0 ; i<6 ; i++ )
            {
                std::cout<<"1 "<<endl;
                //histname=Form("%sSpecColor",IsoMode.c_str());
                //c[j][i]=new TCanvas(histname,"IsoSpec",600,400);
                signalWin[j][i]->SetLineColor(kRed);
                std::cout<<"1.1 "<<endl;
                //histname=Form("%sSpecColor",IsoMode.c_str());
                signalWin[j][i]->GetXaxis()->SetTitle("Energy(MeV)");
                std::cout<<"1.2 "<<endl;
                signalWin[j][i]->GetYaxis()->SetTitle("Entries");
                std::cout<<"1.3 "<<endl;
                signalWin[j][i]->SetStats(kFALSE); 
                std::cout<<"1.4 "<<endl;
                //signalWin[j][i]->Draw();
                offWin[j][i]->SetLineColor(kGreen);
                std::cout<<"1.5 "<<endl;
                offWin[j][i]->SetStats(kFALSE); 
                std::cout<<"1.6 "<<endl;
                //offWin[j][i]->Draw("same");
                histname=Form("%sSpec%sSlice%i_%0.1f_%0.1f",IsoMode.c_str(),ifRed[j].c_str(),i+1,LowEdge4e,HighEdge4e);
                std::cout<<"2 "<<endl;
                isoSpec[j][i]=new TH1F(histname,histname,80,0,20);
                std::cout<<"3 "<<endl;
                isoSpec[j][i]->Sumw2();
                //isoSpec[j][i]->Add(signalWin[j][i],offWin[j][i],1,-0.5);
                std::cout<<"4 "<<endl;
                isoSpec[j][i]->Add(signalWin[j][i],offWin[j][i],1,-1);
                std::cout<<"5 "<<endl;
                isoSpec[j][i]->SetLineColor(kBlue);
                isoSpec[j][i]->SetStats(kFALSE);
                isoSpec[j][i]->SetMarkerStyle(20);
                isoSpec[j][i]->SetMarkerSize(0.7);
                isoSpec[j][i]->SetMarkerColor(kBlue);
                isoSpec[j][i]->SetOption("E");
                //isoSpec[j][i]->Draw("sameE");
                //TLegend *legend=new TLegend(.4,.65,.79,.89);
                //histname=Form("signalWin[j][i] : %0.3f ~ %0.3f",signalWin[j][i]Low,signalWin[j][i]High);
                //legend->AddEntry(signalWin[j][i],histname,"lp");
                //histname=Form("offWin[j][i] : %0.3f ~ %0.3f",offWin[j][i]Low,offWin[j][i]High);
                //legend->AddEntry(offWin[j][i],histname,"lp");
                //histname=Form("%sSpec",IsoMode.c_str());
                //legend->AddEntry(isoSpec[j][i],histname,"lp");
                //legend->SetFillColor(0);
                //legend->Draw("same");

                //THStack* hs = new THStack("hs","");
                //hs->Add(signalWin[j][i]);
                //hs->Add(offWin[j][i]);
                //hs->Draw();
                //isoSpec[j][i]->Draw("same");
                //legend->Draw("same");
                signalWin[j][i]->Write();
                offWin[j][i]->Write();
                isoSpec[j][i]->Write();
                std::cout<<"6 "<<endl;
                singleSpecVsTime[j][i]->Write();
                std::cout<<"7 "<<endl;
                signalWinXY[j][i]->Write();
                offWinXY[j][i]->Write();
                signalWinRZ[j][i]->Write();
                offWinRZ[j][i]->Write();
                //c[j][i]->Write();

            }
        }
    }

    if( genAmCNeu )
    {

        TCanvas *c1=new TCanvas("AmCNeuSpec","AmCNeuSpec",1200,600);
        c1->Divide(2,2);
        for( int i=0 ; i<4 ; i++ )
        {
            c1->cd(i+1);
            singleUpper[i]->SetLineColor(kRed);
            singleUpper[i]->GetXaxis()->SetTitle("Energy(MeV)");
            singleUpper[i]->GetYaxis()->SetTitle("Entries/0.05Mev");
            singleUpper[i]->SetStats(kFALSE);
            //singleUpper[i]->Draw();
            singleLower[i]->SetLineColor(kGreen);
            singleLower[i]->SetStats(kFALSE);
            //singleLower[i]->Draw("same");
            AmCSpec[i]->Sumw2();
            AmCSpec[i]->Add(singleUpper[i],singleLower[i],1,-0.95);
            AmCSpec[i]->SetLineColor(kBlue);
            AmCSpec[i]->SetStats(kFALSE);
            //AmCSpec[i]->Draw("same");
            THStack* hs = new THStack("hs","");
            hs->Add(singleUpper[i]);
            hs->Add(singleLower[i]);
            //hs->Add(AmCSpec[i]);
            hs->Draw();
            AmCSpec[i]->Draw("same");
            TLegend *legend=new TLegend(.4,.65,.79,.89);
            //std::cout<<"singleUpper[i]->FindBin(LowEdge)  : "<<singleUpper[i]->FindBin(LowEdge)<<endl;
            //std::cout<<"singleUpper[i]->FindBin(HighEdge)  : "<<singleUpper[i]->FindBin(HighEdge)<<endl;
            double upperNum=singleUpper[i]->Integral(singleUpper[i]->FindBin(LowEdge),singleUpper[i]->FindBin(HighEdge));
            double lowNum=singleLower[i]->Integral(singleLower[i]->FindBin(LowEdge),singleLower[i]->FindBin(HighEdge));
            histname=Form("Single Upper : %0.0f",upperNum);
            legend->AddEntry(singleUpper[i],histname,"lp");
            histname=Form("Single Lower : %0.0f",lowNum);
            legend->AddEntry(singleLower[i],histname,"lp");
            histname=Form("AmC number : %0.0f",upperNum-lowNum*0.95);
            legend->AddEntry(AmCSpec[i],histname,"lp");
            histname=Form("   %0.1f ~ %0.1f MeV",LowEdge,HighEdge);
            legend->SetHeader(histname);
            legend->SetFillColor(0);
            legend->Draw("same");
            /*
               for( int j=81 ; j<401 ; j++ )
               {
               if( singleUpper[i]->GetBinContent(j)==0 )
               {
               continue;
               }
               std::cout<<"Upper "<<i<<"  : "<<singleUpper[i]->GetBinContent(j)<<endl;
               std::cout<<"Lower "<<i<<"  : "<<singleLower[i]->GetBinContent(j)<<endl;
               std::cout<<"AmC   "<<i<<"  : "<<AmCSpec[i]->GetBinContent(j)<<endl;
               std::cout<<"U-L   "<<i<<"  : "<<singleUpper[i]->GetBinContent(j)-singleLower[i]->GetBinContent(j)*0.95<<endl;
               }
             */
        }

        for( int i=0 ; i<4 ; i++ )
        {
            singleUpper[i]->Write();
            singleLower[i]->Write();
            AmCSpec[i]->Write();
        }
        c1->Write();
    }
    if( genTimeFitDis )
    {
        for( int i=0 ; i<5 ; i++ )
        {
            time2lastshowermuon[i]->Write();
            time2lastshowermuonNoRed[i]->Write();
        }
        time2Allmuon[0]->Write();
        time2Allmuon[1]->Write();
        for( int j=0 ; j<6 ; j++ )
        {
            time2lastmuon[j]->Write();
            time2lastmuonNoRed[j]->Write();
        }

    }

    //c1->Write();
    file->Close();
}
