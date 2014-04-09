{
    //TCanvas *c =new TCanvas("d","d",800,500);
    TFile *f1 = new TFile("/afs/ihep.ac.cn/users/l/lidj/file/IsotopesAna/job/EH1FitResult_P13A.root");
    //TFile *f2 = new TFile("/publicfs/dyb/data/userdata/zhangfh/IbdAna/ana/HeLiAna/P12E/EH1/mergedHist.root");
    TFile *f2 = new TFile("/afs/ihep.ac.cn/users/l/lidj/file/IsotopesAna/job/liuyb/5slice.root");
    TH1F* h1[5];
    TH1F* h2[5];
    TH1F* h3[5];
    TCanvas *c1=new TCanvas("c1","c1",800,900);
    c1->Divide(2,3);
    for( int i=0 ; i<5 ; i++ )
    {
        TString h1Name="slice";
        h1Name+=i+1;
        h1[i]=(TH1F*)f1->Get(h1Name);
        std::cout<<" "<<endl;
        std::cout<<"h1 0-20  : "<<h1[i]->Integral(1,h1[i]->FindBin(20))<<endl;
        //std::cout<<"h1 20  : "<<h1[i]->FindBin(20)<<endl;
        //TString h2Name="zhangfh/time2LastMuonRed";
        TString h2Name="slice";
        h2Name+=i+1;
        h2[i]=(TH1F*)f2->Get(h2Name);
        //std::cout<<"h2 20  : "<<h2[i]->FindBin(20)<<endl;
        //std::cout<<"h2 all  : "<<h2[i]->GetEntries()<<endl;
        double h2entries=h2[i]->Integral(1,h2[i]->FindBin(20));
        std::cout<<"h2 0-20 : "<<h2entries<<endl;
        //double h2entries3=h2[i]->Integral(h2[i]->FindBin(20),h2[i]->FindBin(250));
        //std::cout<<"h2 20-250 : "<<h2entries3<<endl;
        //double h2entries2=h2[i]->Integral(1,h2[i]->FindBin(250));
        //std::cout<<"h2 0-250 : "<<h2entries2<<endl;
        std::cout<<"h2-h1 num  : "<<h2entries-h1[i]->Integral(1,h1[i]->FindBin(20))<<endl;

        //h1[i]->Rebin(10);
        h1[i]->GetXaxis()->SetRange(0,h1[i]->FindBin(20));
        h1[i]->SetMinimum(0);
        //h2[i]->Rebin(20);

        //std::cout<<"h1 20  : "<<h1[i]->FindBin(20)<<endl;
        //std::cout<<"h2 20  : "<<h2[i]->FindBin(20)<<endl;
        TString h3name="h3";
        h3name+=i;
        h3[i]=new TH1F(h3name,"h3",4000,0.001,20.001);
        //std::cout<<"h3 20  : "<<h3[i]->FindBin(20)<<endl;
        //h3[i]->Add(h1[i],h2[i],-1,1);
        for( int j=0 ; j<=4000 ; j++ )
        {
            h3[i]->SetBinContent(j,(h2[i]->GetBinContent(j)-h1[i]->GetBinContent(j)));
        }
        

        std::cout<<"h3 0-20  : "<<h3[i]->Integral(1,h3[i]->FindBin(20))<<endl;
        c1->cd(i+1);
        h1[i]->SetLineColor(kRed);
        h1[i]->Draw();
        h2[i]->SetLineColor(kBlue);
        h2[i]->Draw("same");
        h3[i]->SetLineColor(kGreen);
        h3[i]->Draw("same");
    }
    /*
    
    std::cout<<"h1->GetEntries()  : "<<h1->GetEntries()<<endl;
    std::cout<<"h1 binNum  : "<< h1->GetXaxis()->GetNbins()<<endl;
    //h1->Rebin(10);
    std::cout<<"h1 binNum  : "<< h1->GetXaxis()->GetNbins()<<endl;
    h1->SetLineColor(2);
    //std::cout<<"h1 num 12-20  : "<<h1->Integral(155,200,"width")<<endl; 
    //std::cout<<"h1 num 0.7-12  : "<<h1->Integral(108,154,"width")<<endl; 
    //std::cout<<"h1 num  : "<<h1->Integral(1,200,"width")<<endl; 
    //h1->Draw();
    std::cout<<"h2->GetEntries()  : "<<h2->GetEntries()<<endl;
    std::cout<<"h2 binNum  : "<< h2->GetXaxis()->GetNbins()<<endl;
    h2->Rebin(4);
    std::cout<<"h2 binNum  : "<< h2->GetXaxis()->GetNbins()<<endl;
    //h2->Draw("same");
    
    TH1F* h3=new TH1F(*h1);
    h3->Add(h1,h2,-1,1);
    //std::cout<<"h3 num 12-20  : "<<h3->Integral(155,200,"width")<<endl; 
    //std::cout<<"h3 num 0.7-12  : "<<h3->Integral(1,h3->FindBin(12),"width")<<endl; 
    //std::cout<<"h3 num  : "<<h3->Integral(1,200,"width")<<endl; 
    h3->Draw();
    */
    //TH1F h5=0*(*h1);
    /*
    TH1F* h5=new TH1F(*h1);
    h5->Add(h1,h1,0,0);
        
    for( int i=2 ; i<=200 ; i++ )
    {
        h5->SetBinContent(i,(h3->Integral(1,i,"width")-h3->Integral(1,i-1,"width")));
    }
    //h5->Draw("same");
    h5->Draw();
    */
}
