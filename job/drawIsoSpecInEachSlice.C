{
    string davaVer="P14A";
    string IsoMode="B12";
    TH1F* h[2][6];
    TCanvas *c[2];
    TString histname;
    string ifRed[2];
    ifRed[0]="NoRed";
    ifRed[1]="";
    double LowEdge4e=5.;
    double HighEdge4e=20.;
    int site;
    for( int k=0 ; k<3 ; k++ )
    {
        site=k+1;
        histname=Form("%s/EH%iiso_%s.root",davaVer.c_str(),site,davaVer.c_str());
    TFile* f=new TFile(histname);
        if( f->IsZombie() )
        {
            std::cout<<"Can't open file : "<<histname<<endl;
        }
        for( int j=0 ; j<2 ; j++ )
        {
            histname=Form("IsoSpecInEachSlice%s",ifRed[j].c_str());
            c[j] = new TCanvas(histname,histname,1800,900);
            c[j]->Divide(3,2);
            for( int i=0 ; i<6 ; i++ )
            {
                histname=Form("%sSpec%sSlice%i_%0.1f_%0.1f",IsoMode.c_str(),ifRed[j].c_str(),i+1,LowEdge4e,HighEdge4e);
                h[j][i]=(TH1F*)f->Get(histname);
                if( !h[j][i] )
                {
                    std::cout<<"Can't find  : "<<histname<<endl;
                }
                c[j]->cd(i+1);
                h[j][i]->Draw();
            }
            histname=Form("P14A/dataEps/EH%i%sSpec%s_%0.1f_%0.1f.eps",site,IsoMode.c_str(),ifRed[j].c_str(),LowEdge4e,HighEdge4e);
            c[j]->SaveAs(histname);
            delete c[j];
            c[j]=NULL;
        }
        f->Close();
    }
}
