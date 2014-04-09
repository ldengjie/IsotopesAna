{
    //TCanvas *c =new TCanvas("d","d",800,500);
    TFile *f1 = new TFile("/afs/ihep.ac.cn/users/l/lidj/file/IsotopesAna/job/EH1totalHisto_P13A.root");
    //TFile *f2 = new TFile("/publicfs/dyb/data/userdata/zhangfh/IbdAna/ana/HeLiAna/P12E/EH1/mergedHist.root");
    //TFile *f2 = new TFile("/afs/ihep.ac.cn/users/l/lidj/file/IsotopesAna/job/liuyb/5slice.root");
    TFile *f2 = new TFile("/afs/ihep.ac.cn/users/l/lidj/file/IsotopesAna/job/liuyb/7slice_liuyb.root");
    TString h2name[5];
    h2name[0]="EH1/Muon20to60/Time2LastMuon_noReduction_ads_Muon20to60";
    h2name[1]="EH1/Muon60to500/Time2LastMuon_noReduction_ads_Muon60to500";
    h2name[2]="EH1/Muon500to1500/Time2LastMuon_noReduction_ads_Muon500to1500";
    h2name[3]="EH1/Muon1500to2500/Time2LastMuon_noReduction_ads_Muon1500to2500";
    h2name[4]="EH1/Muon2500to3500/Time2LastMuon_noReduction_ads_Muon2500to3500";
    TH1F* h1[5];
    TH1F* h10[5];
    TH1F* h2[5];
    TH1F* h3[5];
    TCanvas *c1=new TCanvas("c1","c1",800,900);
    c1->Divide(2,3);
    for( int i=0 ; i<5 ; i++ )
    {
        TString h1Name="lidj/time2lastshowermuonNoRed";
        h1Name+=i+1;
        h10[i]=(TH1F*)f1->Get(h1Name);
        h1[i]= new TH1F("h1","h1",17999,1,18000);
        for( int j=1 ; j<18000 ; j++ )
        {
            h1[i]->SetBinContent(j,h10[i]->GetBinContent(j+1));
        }
        h1[i]->Rebin(5);
        
        std::cout<<" "<<endl;
        std::cout<<"h1 0-18000  : "<<h1[i]->Integral(1,h1[i]->FindBin(18000))<<endl;

        h2[i]=(TH1F*)f2->Get(h2name[i]);
        double h2entries=h2[i]->Integral(1,h2[i]->FindBin(18000));
        std::cout<<"h2 0-18000 : "<<h2entries<<endl;
        std::cout<<"h2-h1 num  : "<<h2entries-h1[i]->Integral(1,h1[i]->FindBin(18000))<<endl;

        //h1[i]->GetXaxis()->SetRange(0,h1[i]->FindBin(18000));
        //h1[i]->SetMinimum(0);

        TString h3name="h3";
        h3name+=i;
        h3[i]=new TH1F(h3name,"h3",3600,1,18001);
        for( int j=0 ; j<=3600 ; j++ )
        {
            h3[i]->SetBinContent(j,(h2[i]->GetBinContent(j)-h1[i]->GetBinContent(j)));
        }
        

        std::cout<<"h3 0-18000  : "<<h3[i]->Integral(1,h3[i]->FindBin(18000))<<endl;
        c1->cd(i+1);
        h1[i]->SetLineColor(kRed);
        h1[i]->Draw();
        h2[i]->SetLineColor(kBlue);
        h2[i]->Draw("same");
        h3[i]->SetLineColor(kGreen);
        h3[i]->Draw("same");
    }
}
