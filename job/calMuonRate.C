{
    //TFile* f=new TFile("EH1totalHisto_P14A_mulitSlice.root");
    //TFile* f=new TFile("EH2totalHisto_P14A_mulitSlice.root");
    TFile* f=new TFile("EH3totalHisto_P14A_mulitSlice.root");
    TH1F* h =(TH1F*)f->Get("lidj/muonSpecNoRed");
    int binNum=h->GetNbinsX();
    for( int i=1 ; i<=binNum ; i++ )
    {
        //h->SetBinContent(i,h->GetBinContent(i)/(1140*24*3600));
        //h->SetBinContent(i,h->GetBinContent(i)/(956.1*24*3600));
        h->SetBinContent(i,h->GetBinContent(i)/(2079.3*24*3600));
    }
    
    h->Draw();
}
