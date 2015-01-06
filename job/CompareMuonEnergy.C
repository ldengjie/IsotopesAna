{
    string nameStr;
    TCanvas* c[3];
    TFile* f[3];
    for( int i=0 ; i<3 ; i++ )
    {
        nameStr=Form("/afs/ihep.ac.cn/users/l/lidj/file/IsotopesAna/data/P14A/EH%d/P14AEH%d_hists.root",i+1,i+1);
        cout<<"path  : "<<nameStr<<endl;
        f[i]= new TFile(nameStr.c_str());
        TH1F* h6=(TH1F*)f[i]->Get("lidj/muonEnergy6");
        TH1F* hI16=(TH1F*)f[i]->Get("lidj/muonEnergyI16");
        TH1F* hI26=(TH1F*)f[i]->Get("lidj/muonEnergyI26");
        TH1F* hI36=(TH1F*)f[i]->Get("lidj/muonEnergyI36");
        TH1F* hNoRed6=(TH1F*)f[i]->Get("lidj/muonEnergyNoRed6");
        h6->Rebin(10);
        hI16->Rebin(10);
        hI26->Rebin(10);
        hI36->Rebin(10);
        hNoRed6->Rebin(10);
        h6->GetXaxis()->SetRange(0,700);
        nameStr=Form("Reductions of Muon in EH%d",i+1);
        h6->SetTitle(nameStr.c_str());
        h6->GetXaxis()->SetTitle("Muon energy /MeV");
        h6->GetYaxis()->SetTitle("Reduction fraction");

        h6->Add(h6,hNoRed6,-1,1);
        h6->Divide(hNoRed6);
        hI16->Add(hI16,hNoRed6,-1,1);
        hI16->Divide(hNoRed6);
        hI26->Add(hI26,hNoRed6,-1,1);
        hI26->Divide(hNoRed6);
        hI36->Add(hI36,hNoRed6,-1,1);
        hI36->Divide(hNoRed6);
        h6->SetLineColor(kRed);
        hI16->SetLineColor(kBlue);
        hI26->SetLineColor(kGreen);
        hI36->SetLineColor(6);
        nameStr=Form("EH%d",i+1);
        c[i]=new TCanvas(nameStr.c_str(),nameStr.c_str(),800,600);
        h6->SetStats(0);
        hI16->SetStats(0);
        hI26->SetStats(0);
        hI36->SetStats(0);
        h6->Draw();
        hI16->Draw("same");
        hI26->Draw("same");
        hI36->Draw("same");
        double hI1frac=(hI16->GetBinContent(22)+hI16->GetBinContent(23)+hI16->GetBinContent(24)+hI16->GetBinContent(25)+hI16->GetBinContent(26))/5;
        double hI2frac=(hI26->GetBinContent(22)+hI26->GetBinContent(23)+hI26->GetBinContent(24)+hI26->GetBinContent(25)+hI26->GetBinContent(26))/5;
        double hI3frac=(hI36->GetBinContent(22)+hI36->GetBinContent(23)+hI36->GetBinContent(24)+hI36->GetBinContent(25)+hI36->GetBinContent(26))/5;
        TLegend *legend=new TLegend(.68,.77,.99,.99);
        legend->AddEntry(h6,"Normal Muon Reduction","lp");
        nameStr=Form("Time interval > 100ms , %2.2f%%",hI1frac*100);
        legend->AddEntry(hI16,nameStr.c_str(),"lp");
        nameStr=Form("Time interval > 200ms , %2.2f%%",hI2frac*100);
        legend->AddEntry(hI26,nameStr.c_str(),"lp");
        nameStr=Form("Time interval > 300ms , %2.2f%%",hI3frac*100);
        legend->AddEntry(hI36,nameStr.c_str(),"lp");
        legend->Draw("same");
        nameStr=Form("CompareMuonEnergyEH%d.eps",i+1);
        c[i]->SaveAs(nameStr.c_str());
        
    }
    
}
