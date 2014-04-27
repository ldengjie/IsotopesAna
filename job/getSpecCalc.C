{
    TFile* f = new TFile("Model1.root");
    TGraph* g=(TGraph*)f->Get("electronFull");
    double *x=g->GetX();
    double *y=g->GetY();
    //for( int i=0 ; i<300 ; i++ )
    //{
    //std::cout<<"["<<i<<"]  : "<<x[i]<<" "<< y[i]<<endl;
    //}
    //std::cout<<"["<<300<<"]  : "<<x[300]<<" "<< y[300]<<endl;
    //exit();
    //g->Draw();
    int pNum=g->GetN();
    std::cout<<"point num  : "<<pNum<<endl;
    TFile* f1=new TFile("IsoTheoreticalSpec.root","update");
    if( f1->IsZombie() )
    {
        std::cout<<"Error : can't open IsoTheoreticalSpec.root "<<endl;
        exit();
    }
    TList* l = gDirectory->GetListOfKeys() ;
    TIter next(l);
    TKey* key;
    TObject* obj ;
    TH1F* h1;
    TH1F* h2;
    vector<string> delHistList;
    //delete old histograms,this is necessary
    while ( key = (TKey*)next() ) 
    {
        obj = key->ReadObj() ;
        if ((strcmp(obj->IsA()->GetName(),"TProfile")!=0) && (!obj->InheritsFrom("TH2")) && (!obj->InheritsFrom("TH1"))) 
        {
            printf("<W> Object %s is not 1D or 2D histogram : "
                    "will not be converted\n",obj->GetName()) ;
        }
        h1 = (TH1F*)obj;
        printf("Histo name:%s title:%s\n",h1->GetName(),h1->GetTitle());
        string h1name=h1->GetName();
        if( h1name.find("After")!=string::npos )
        {
            //gDirectory->Delete(h1name.c_str());
            //std::cout<<"Done delete "<<endl;
            h1name+=";*";
            delHistList.push_back(h1name);
        }
        //std::cout<<"next "<<endl;
        delete h1;
    }
    std::cout<<"delHistList size  : "<<delHistList.size()<<endl;
    if( delHistList.size()>0 )
    {
        for( vector<string>::iterator it=delHistList.begin() ; it!=delHistList.end() ; it++ )
        {
            std::cout<<"deleting  : "<<(*it)<<endl;
            gDirectory->Delete((*it).c_str());
        }
    }
    
    std::cout<<" "<<endl;
    std::cout<<" "<<endl;
    //count origin spectrum histograms numbers
    TIter next(l);
    int originNum=0;
    while ( key = (TKey*)next() ) 
    {
        obj = key->ReadObj() ;
        if ((strcmp(obj->IsA()->GetName(),"TProfile")!=0) && (!obj->InheritsFrom("TH2")) && (!obj->InheritsFrom("TH1"))) 
        {
            printf("<W> Object %s is not 1D or 2D histogram : "
                    "will not be converted\n",obj->GetName()) ;
        }
        h1 = (TH1F*)obj;
        printf("Histo name:%s title:%s\n",h1->GetName(),h1->GetTitle());
        originNum++;
        delete h1;
    }
    std::cout<<"origin spectrum histograms number  : "<<originNum<<endl;
    std::cout<<" "<<endl;
    std::cout<<" "<<endl;
    //do correction!
    int originNumTmp=originNum;
    TIter next(l);
    while ( key = (TKey*)next()) 
    {
        if(originNum<=0) break;
        originNum--;
        obj = key->ReadObj() ;
        if ((strcmp(obj->IsA()->GetName(),"TProfile")!=0) && (!obj->InheritsFrom("TH2")) && (!obj->InheritsFrom("TH1"))) 
        {
            printf("<W> Object %s is not 1D or 2D histogram : "
                    "will not be converted\n",obj->GetName()) ;
        }
        h1 = (TH1F*)obj;
        printf("Histo name:%s title:%s\n",h1->GetName(),h1->GetTitle());
        TString h2name=h1->GetName();
        h2name+="AfterCor";
        TString h2title=h1->GetTitle();
        h2title+=" After Correction";
        std::cout<<"h2name  : "<<h2name<<endl;
        std::cout<<"h2title  : "<<h2title<<endl;
        TString h2nameDel=h2name+";*";
        gDirectory->Delete(h2nameDel);
        int h1BinNum=h1->GetNbinsX();
        double h1xlow=h1->GetXaxis()->GetXmin();
        double h1xhigh=h1->GetXaxis()->GetXmax();
        h2=new TH1F(h2name,h2title,h1BinNum,h1xlow,h1xhigh);
        std::cout<<"h1BinNum  : "<<h1BinNum<<endl;

        double cof=1.;
        double xcor=0.;
        int corNum=4000;
        double xnow=0.;
        int newbinNum;
        for( int i=1 ; i<=h1BinNum ; i++ )
        {
            if(i%1000==0) std::cout<<"now is  : "<<i<<"/"<<h1BinNum<<"    "<<originNumTmp-originNum<<"/"<<originNumTmp <<endl;
            double xmid=h1->GetBinCenter(i);
            double binLowEdge=h1->GetBinLowEdge(i);
            double binWidth=h1->GetBinWidth(i);
            double binContent=h1->GetBinContent(i);
            if( xmid<x[299] )
            {
                for( int j=0 ; j<299 ; j++ )
                {
                    if( xmid>x[j]&&xmid<x[j+1] )
                    {
                        for( int k=0 ; k<corNum ; k++ )
                        {
                            xnow=binLowEdge+(binWidth/corNum)*(1/2+k);
                            cof=y[j]+(y[j+1]-y[j])*(xnow-x[j])/(x[j+1]-x[j]);
                            xcor=(xnow)*cof;
                            newbinNum=h2->FindBin(xcor);
                            h2->AddBinContent(newbinNum,binContent/corNum);
                        }
                        break;
                    }
                }

            }else
            {
                for( int k=0 ; k<corNum ; k++ )
                {
                    xnow=binLowEdge+(binWidth/corNum)*(1/2+k);
                    xcor=(xnow)*cof;
                    newbinNum=h2->FindBin(xcor);
                    h2->AddBinContent(newbinNum,binContent/corNum);
                }
            }
            //if( i<50 )
            //{
            //std::cout<<"old -> new "<<i<<" -> "<<newbinNum<<endl;
            //}

        }
        h1->SetLineColor(kRed);
        //h1->Draw("same");
        h2->SetLineColor(kBlue);
        //h2->Draw("same");
        h2->Write();
        delete h1;
        delete h2;
    }
    //TH1F* h3=new TH1F("LiSpec_12bin","LiSpec after cor",12,0,12);
    //for( int i=0 ; i<12 ; i++ )
    //{
    //double binContent=0.;
    //for( int j=i*100 ; j<(i+1)*100 ; j++ )
    //{
    //if(j>70) binContent+=h2->GetBinContent(j+1);
    //}
    //h3->SetBinContent(i+1,binContent);
    //}
    //h3->Scale(1/h3->Integral());
    //h3->SetLineColor(kGreen);
    //h3->Draw();
    //h3->Write();
    //f1->Close();

}
