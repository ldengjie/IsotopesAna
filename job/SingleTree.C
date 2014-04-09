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
    TString exRateStr=option(10,4);
    double exRate=exRateStr.Atof();
    genIsoSpec=0;
    genAmCNeu=0;
    genTimeFitDis=1;
    //IsoMode="C9";//Li8,C99,B12
    IsoMode=IsoModeTmp;//Li8,C99,B12
    for( int i=0 ; i<(BINNUM*2+4) ; i++ )
    {
        T2lastMuon[i]=0.;
    }

    for( int i=0 ; i<BINNUM ; i++ )
    {
        muonRate[i]=0.;
        muonEnergy[i]=0.;
        muonEnergyNoRed[i]=0.;
        muonRateNoRed[i]=0.;
    }
    tlow=0.001;
    thigh=12.;
    if( IsoMode=="B12" )
    {
        LowEdge=4.0;
        HighEdge=20.0;
        RATETHR=exRate;//hist for B12
        LowEdge4e=LowEdge;
        HighEdge4e=HighEdge;
        signalWinLow=0.002;          
        signalWinHigh=0.06;
        offWinLow=0.502;
        offWinHigh=0.560;
        //thigh=0.5;
    }else if( IsoMode=="Li8" )
    {
        LowEdge=4.0;//for N12 time fit,that for Li8 has been done while B12.
        HighEdge=20.0;
        RATETHR=exRate;//hist for Li8
        LowEdge4e=4.0;
        HighEdge4e=20.0;
        signalWinLow=0.6;
        signalWinHigh=4.0;
        offWinLow=10.6;
        offWinHigh=14.0;
        //thigh=10.;
    } else if( IsoMode=="C99" )
    {
        LowEdge=12.0;
        HighEdge=20.0;
        RATETHR=exRate;//hist for C9
        LowEdge4e=LowEdge;
        HighEdge4e=HighEdge;
        signalWinLow=0.2;
        signalWinHigh=0.6;
        offWinLow=10.2;
        offWinHigh=10.6;
        //thigh=2.5;
    }else if( IsoMode=="N12" )
    {

        //thigh=0.5;
        LowEdge=14.0;
        HighEdge=20.0;
        RATETHR=exRate;
        LowEdge4e=LowEdge;
        HighEdge4e=HighEdge;
        signalWinLow=0.002;
        signalWinHigh=0.06;
        offWinLow=0.502;
        offWinHigh=0.560;
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
    double totaldaq=0.;
    if( option=="EH1")
    {
        site=0;
        ADNum=2;
        totaldaq=1140.;
    }else if( option=="EH2" )
    {
        site=1;
        ADNum=daqHistNum-3;
        totaldaq=956.1;
    }else
    {
        totaldaq=2079.3;
    }


    TFile *f=new TFile(Form("%sTime_%s_multiSlice.root",option.Data(),dataVer.Data()));
    TH1F* hNoRed=(TH1F*)f->Get("lidj/muonSpecNoRed");
    TH1F* h=(TH1F*)f->Get("lidj/muonSpec");

    double rateThr=0.;
    sliceNumNoRedTmp=0;
    for( int i=0 ; i<BINNUM ; i++ )
    {
        rateThr+=hNoRed->GetBinContent(i+1)/(totaldaq*24*3600);
        if( rateThr>=RATETHR ||i==(BINNUM-1))
        {
            sliceNumNoRed[sliceNumNoRedTmp]=i;
            muonRateNoRed[sliceNumNoRedTmp]=rateThr;
            if( i<=979 )
            {
                muonEnergyNoRed[sliceNumNoRedTmp]=i+20+1;
            } else
            {
                muonEnergyNoRed[sliceNumNoRedTmp]=(i-979)*5+1000;
            }
            sliceNumNoRedTmp++;
            rateThr=0.;
        }
    }
    std::cout<<"[" <<IsoMode <<"]" <<"slice information w/o muon reduction "<<endl;
    for( int i=0 ; i<sliceNumNoRedTmp ; i++ )
    {
        std::cout<<Form("%4i  : %4i %f %.0f",i,sliceNumNoRed[i],muonRateNoRed[i],muonEnergyNoRed[i])<<endl;
    }
    std::cout<<"[" <<IsoMode <<"]" <<"slice information w/o muon reduction "<<endl;

    sliceNumTmp=0;
    for( int i=0 ; i<BINNUM ; i++ )
    {
        rateThr+=h->GetBinContent(i+1)/(totaldaq*24*3600);
        if( rateThr>=RATETHR ||i==(BINNUM-1))
        {
            sliceNum[sliceNumTmp]=i;
            muonRate[sliceNumTmp]=rateThr;
            if( i<=979 )
            {
                muonEnergy[sliceNumTmp]=i+20+1;
            } else
            {
                muonEnergy[sliceNumTmp]=(i-979)*5+1000;
            }
            sliceNumTmp++;
            rateThr=0.;
        }
    }
    std::cout<<"[" <<IsoMode <<"]" <<"slice information w/ muon reduction "<<endl;
    for( int i=0 ; i<sliceNumTmp ; i++ )
    {
        std::cout<<Form("%4i  : %4i %f %.0f",i,sliceNum[i],muonRate[i],muonEnergy[i])<<endl;
    }
    std::cout<<"[" <<IsoMode <<"]" <<"slice information w/ muon reduction "<<endl;
    option+="Data_";
    option+=dataVer;
    option+="_multiSlice.root";
    TString savePath="/afs/ihep.ac.cn/users/l/lidj/largedata/IsotopesAna/job/";
    savePath+=option;

    file = new TFile(savePath,"update");
    if( genTimeFitDis )
    {

        gDirectory->Delete(Form("sliceNumNoRed_%0.2f_%0.1f_%0.1f;*",RATETHR,LowEdge,HighEdge));
        sliceNumHNoRed=new TH1F(Form("sliceNumNoRed_%0.2f_%0.1f_%0.1f",RATETHR,LowEdge,HighEdge),"muon rate in each slice w/o muon reduction",BINNUM,0,BINNUM);
        sliceNumHNoRed->SetXTitle("Slice Number");
        sliceNumHNoRed->SetYTitle("Muon Rate in each Slice");
        gDirectory->Delete(Form("sliceNum_%0.2f_%0.1f_%0.1f;*",RATETHR,LowEdge,HighEdge));
        sliceNumH=new TH1F(Form("sliceNum_%0.2f_%0.1f_%0.1f",RATETHR,LowEdge,HighEdge),"muon rate in each slice",BINNUM,0,BINNUM);
        sliceNumH->SetXTitle("Slice Number");
        sliceNumH->SetYTitle("Muon Rate in each Slice");
        for( int i=0 ; i<sliceNumTmp ; i++ )
        {
            gDirectory->Delete(Form("time2lastshowermuon_%0.2f_%i_%0.1f_%0.1f;*",RATETHR,i+1,LowEdge,HighEdge));
            histname=Form("time2lastshowermuon_%0.2f_%i_%0.1f_%0.1f",RATETHR,i+1,LowEdge,HighEdge);
            time2lastshowermuon[i]=new TH1F(histname,"time2lastshowermuon",99999,0.001,100);
            time2lastshowermuon[i]->SetXTitle("Time (S)");
            time2lastshowermuon[i]->SetYTitle("Entries");
        }
        for( int i=0 ; i<sliceNumNoRedTmp ; i++ )
        {

            gDirectory->Delete(Form("time2lastshowermuonNoRed_%0.2f_%i_%0.1f_%0.1f;*",RATETHR,i+1,LowEdge,HighEdge));
            histname=Form("time2lastshowermuonNoRed_%0.2f_%i_%0.1f_%0.1f",RATETHR,i+1,LowEdge,HighEdge);
            time2lastshowermuonNoRed[i]=new TH1F(histname,"time2lastshowermuon w/o muon reduction",99999,0.001,100);
            time2lastshowermuonNoRed[i]->SetXTitle("Time (S)");
            time2lastshowermuonNoRed[i]->SetYTitle("Entries");
        }

        //histname=Form("time2Allmuon_%0.1f_%0.1f",LowEdge,HighEdge);
        //time2Allmuon=new TH1F(histname,"time2Allmuon",99999,0.001,100);
        //histname=Form("time2AllmuonNoRed_%0.1f_%0.1f",LowEdge,HighEdge);
        //time2AllmuonNoRed=new TH1F(histname,"time2Allmuon",99999,0.001,100);
        for( int i=0 ; i<sliceNumTmp; i++ )
        {
            gDirectory->Delete(Form("slice_%0.2f_%i_%0.1f_%0.1f;*",RATETHR,i+1,LowEdge,HighEdge));
            time2lastmuon[i]=new TTree(Form("slice_%0.2f_%i_%0.1f_%0.1f",RATETHR,i+1,LowEdge,HighEdge),"time2lastmuon");

            time2lastmuon[i]->Branch("xt",&minTime[i],"xt/D");
        }
        for( int i=0 ; i<sliceNumNoRedTmp; i++ )
        {
            gDirectory->Delete(Form("sliceNoRed_%0.2f_%i_%0.1f_%0.1f;*",RATETHR,i+1,LowEdge,HighEdge));
            time2lastmuonNoRed[i]=new TTree(Form("sliceNoRed_%0.2f_%i_%0.1f_%0.1f",RATETHR,i+1,LowEdge,HighEdge),"time2lastmuon NoRed");

            time2lastmuonNoRed[i]->Branch("xt",&minTimeNoRed[i],"xt/D");
        }
        /*       
                 time2lastmuon[5]=new TTree(Form("slice6_%0.1f_%0.1f",LowEdge,HighEdge),"time2lastmuon");
                 time2lastmuon[5]->Branch("xt",&minT2allmuon,"xt/F");
                 time2lastmuonNoRed[5]=new TTree(Form("sliceNoRed6_%0.1f_%0.1f",LowEdge,HighEdge),"time2lastmuon NoRed");
                 time2lastmuonNoRed[5]->Branch("xt",&minT2allmuonNoRed,"xt/F");
         */
    }
    if( genAmCNeu )
    {
        for( int i=0 ; i<4 ; i++ )
        {
            //histname=Form("singleSpecAD%i_%0.1f_%0.1f",i+1,LowEdge,HighEdge);
            //singleSpec[i]=new TH1F(histname,"spectra of singles",400,0,20);
            gDirectory->Delete(Form("singleUpperAD%i_%0.1f_%0.1f;*",i+1,LowEdge,HighEdge));
            histname=Form("singleUpperAD%i_%0.1f_%0.1f",i+1,LowEdge,HighEdge);
            singleUpper[i]=new TH1F(histname,"single upper",400,0,20);
            gDirectory->Delete(Form("singleLowerAD%i_%0.1f_%0.1f;*",i+1,LowEdge,HighEdge));
            histname=Form("singleLowerAD%i_%0.1f_%0.1f",i+1,LowEdge,HighEdge);
            singleLower[i]=new TH1F(histname,"single lower",400,0,20);
            gDirectory->Delete(Form("AmCSpecAD%i_%0.1f_%0.1f;*",i+1,LowEdge,HighEdge));
            histname=Form("AmCSpecAD%i_%0.1f_%0.1f",i+1,LowEdge,HighEdge);
            AmCSpec[i]=new TH1F(histname,"AmC spectrum",400,0,20);
        }

    }

    if( genIsoSpec )
    {
        histname="signalWindow";
        signalWin=new TH1F(histname,"signalwidow",80,0,20);
        histname="offWindow";
        offWin=new TH1F(histname,"offwidow",80,0,20);

    }

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
        std::cout<<"entry : "<<entry<<endl;
    }
    //B12 spectrum
    if( genIsoSpec )
    {
        //singleSpec[det-1]->Fill(energy);
        //if( energy<3.0 )
        //{
        //return kTRUE;
        //}
        if( energy>LowEdge4e&&energy<HighEdge4e )
        {
            if( t2lastshowermuon>signalWinLow &&t2lastshowermuon<signalWinHigh )
            {
                //if( (x*x+y*y<1550*1550)&&(z*z<1550*1550) )
                //{
                signalWin->Fill(energy);
                //}
            }
            if( t2lastshowermuon>offWinLow &&t2lastshowermuon<offWinHigh )
            {
                //if( (x*x+y*y<1550*1550)&&(z*z<1550*1550) )
                //{
                offWin->Fill(energy);
                //}
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
        for( int i=0 ; i<BINNUM ; i++ )
        {
            minTime[i]=1000000.;
            minTimeNoRed[i]=1000000.;
        }
        for( int i=0 ; i<BINNUM ; i++ )
        {
            if(T2lastMuon[i+4+BINNUM]>thigh) continue;

            for( int j=0 ; j<sliceNumTmp ; j++ )
            {
                if( i<=sliceNum[j] )
                {
                    minTime[j]=T2lastMuon[i+4+BINNUM]<minTime[j]?T2lastMuon[i+4+BINNUM]:minTime[j];
                    break;
                } 

            }

            for( int j=0 ; j<sliceNumNoRedTmp ; j++ )
            {
                if( i<=sliceNumNoRed[j] )
                {
                    minTimeNoRed[j]=T2lastMuon[i+4]<minTimeNoRed[j]?T2lastMuon[i+4]:minTimeNoRed[j];
                    break;
                } 
            }
        }
        for( int j=0 ; j<sliceNumTmp ; j++ )
        {
            time2lastshowermuon[j]->Fill(minTime[j]);
            if(minTime[j]<thigh) time2lastmuon[j]->Fill();
        }
        for( int j=0 ; j<sliceNumNoRedTmp ; j++ )
        {
            time2lastshowermuonNoRed[j]->Fill(minTimeNoRed[j]);
            if(minTimeNoRed[j]<thigh)  time2lastmuonNoRed[j]->Fill();
        }

        /*
           minT2allmuon=1000.;
           minT2allmuonNoRed=1000.;
           for( int j=0 ; j<5 ; j++ )
           {
           minT2allmuon=minT2allmuon<T2lastMuon[j+4]?minT2allmuon:T2lastMuon[j+4];
           minT2allmuonNoRed=minT2allmuonNoRed<T2lastMuon[j+10]?minT2allmuonNoRed:T2lastMuon[j+10];
           }
         */
        //if( T2lastMuon[2+10]>0 )
        //{
        //std::cout<<"find one "<<T2lastMuon[2+10]<<endl;
        //}
        //time2Allmuon->Fill(minT2allmuon);
        //time2AllmuonNoRed->Fill(minT2allmuonNoRed);
        /*
           for( int j=0 ; j<6 ; j++ )
           {
           }
         */


    }


    return kTRUE;
}

void SingleTree::SlaveTerminate()
{

}

void SingleTree::Terminate()
{

    if( genIsoSpec )
    {

        gDirectory->Delete(Form("%sSpecColor;*",IsoMode.c_str()));
        histname=Form("%sSpecColor",IsoMode.c_str());
        TCanvas *c0=new TCanvas(histname,"IsoSpec",600,400);
        signalWin->SetLineColor(kRed);
        signalWin->GetXaxis()->SetTitle("Energy(MeV)");
        signalWin->GetYaxis()->SetTitle("Entries");
        signalWin->SetStats(kFALSE); 
        //signalWin->Draw();
        offWin->SetLineColor(kGreen);
        offWin->SetStats(kFALSE); 
        //offWin->Draw("same");
        gDirectory->Delete(Form("%sSpec_%0.1f_%0.1f;*",IsoMode.c_str(),LowEdge4e,HighEdge4e));
        histname=Form("%sSpec_%0.1f_%0.1f",IsoMode.c_str(),LowEdge4e,HighEdge4e);
        TH1F* B12Spec=new TH1F(histname,"IsoSpec",80,0,20);
        B12Spec->Sumw2();
        //B12Spec->Add(signalWin,offWin,1,-0.5);
        B12Spec->Add(signalWin,offWin,1,-1);
        B12Spec->SetLineColor(kBlue);
        B12Spec->SetStats(kFALSE);
        B12Spec->SetMarkerStyle(20);
        B12Spec->SetMarkerSize(0.7);
        B12Spec->SetMarkerColor(kBlue);
        B12Spec->SetOption("E");
        //B12Spec->Draw("sameE");
        TLegend *legend=new TLegend(.4,.65,.79,.89);
        histname=Form("signalWin : %0.3f ~ %0.3f",signalWinLow,signalWinHigh);
        legend->AddEntry(signalWin,histname,"lp");
        histname=Form("offWin : %0.3f ~ %0.3f",offWinLow,offWinHigh);
        legend->AddEntry(offWin,histname,"lp");
        histname=Form("%sSpec",IsoMode.c_str());
        legend->AddEntry(B12Spec,histname,"lp");
        legend->SetFillColor(0);
        //legend->Draw("same");

        THStack* hs = new THStack("hs","");
        hs->Add(signalWin);
        hs->Add(offWin);
        hs->Draw();
        B12Spec->Draw("same");
        legend->Draw("same");

        //singleSpec[0]->SetOption("E");
        //singleSpec[0]->SetLineColor(kBlue);
        //singleSpec[0]->Draw();
        //singleSpec[1]->SetOption("E");
        //singleSpec[1]->SetLineColor(kRed);
        //singleSpec[1]->Draw("same");
        //for( int i=0 ; i<4 ; i++ )
        //{
        //singleSpec[i]->Write();
        //}

        B12Spec->Write();
        c0->Write();
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
        sliceNumH->SetBinContent(1,sliceNumTmp);
        for( int i=2 ; i<=sliceNumTmp+1 ; i++ )
        {
            sliceNumH->SetBinContent(i,(muonRate[i-2]+muonEnergy[i-2]*10));    
        }

        sliceNumH->Write();
        sliceNumHNoRed->SetBinContent(1,sliceNumNoRedTmp);
        for( int i=2 ; i<=sliceNumNoRedTmp+1 ; i++ )
        {
            sliceNumHNoRed->SetBinContent(i,(muonRateNoRed[i-2]+muonEnergyNoRed[i-2]*10));    
        }

        sliceNumHNoRed->Write();
        for( int i=0 ; i<sliceNumTmp ; i++ )
        {
            time2lastshowermuon[i]->Write();
            time2lastmuon[i]->Write();
        }
        for( int i=0 ; i<sliceNumNoRedTmp ; i++ )
        {
            time2lastshowermuonNoRed[i]->Write();
            time2lastmuonNoRed[i]->Write();
        }

        //time2Allmuon->Write();
        //time2AllmuonNoRed->Write();
        /*
           for( int j=0 ; j<6 ; j++ )
           {
           }
         */
    }

    //c1->Write();
    file->Close();
}
