{
    using namespace RooFit;
    RooRealVar x("x","x",3,16); 
    RooRealVar mean("mean","mean",7,5,15); 
    RooRealVar sigma("sigma","sigma",2,0.,4.); 
    RooGaussian sig("sig","signal p.d.f.",x,mean,sigma); 

    RooRealVar fsig("fsig","signal fraction",0.5,0.,1.); 

    RooRealVar mean2("mean2","mean",15,10,20); 
    RooRealVar sigma2("sigma2","sigma",2,0.,4.); 
    RooGaussian sig2("sig2","signal p.d.f.",x,mean2,sigma2); 
    RooAddPdf model("model","model",RooArgList(sig,sig2),fsig);

    //RooRealVar tau("tau","tau",-1,-50,-0.0); 
    //RooExponential bkg("model","model",x,tau);

    ////RooAddPdf model("model","model",RooArgList(sig,bkg),fsig);
    //RooAddPdf model("model","model",RooArgList(bkg,sig),fsig);
    //RooDataSet* data = model.generate(x,10000);

    //RooRealVar nsig("nsig","signal fraction",50000,0.,70000.); 
    //RooRealVar nbkg("nbkg","background fraction",150000,0.,400000.); 
    //RooAddPdf model("model","model",RooArgList(sig,sig2),RooArgList(nsig,nbkg));
    //RooDataSet* data = model.generate(x);
    //TFile* f=new TFile("testRange.root","recreate");
    //TH1* h=data->createHistogram("histo",x,Binning(200,0,20));
    //h->Draw();
    //h->Write();
    TFile f("testRange.root");
    TH1* h=(TH1*)f.Get("histo__x");
    //h->Draw();
    //x.setRange(4,15);
    x.setRange(0,20);
    //x.setRange(3,16);
    RooDataHist* data=new RooDataHist("data","data",x,h); 

    x.setRange("ref",6,14);
    x.setRange("ref0",3,16);
    x.setRange("coeref",0,20);
    x.setRange("ref2",4,15);
    x.setRange("ref3",5,13);
    //model.fitTo(*data,SumCoefRange("coeref"));
    //model.fitTo(*data,Range("ref"),SumCoefRange("ref"));
    //model.fitTo(*data,Range("ref0"),SumCoefRange("coeref"));
    //model.fitTo(*data,Range("ref"));
    //model.fitTo(*data,Range("ref"),SumCoefRange("coeref"));
    //model.fitTo(*data,Range("ref2"));
    model.fitTo(*data,Range("ref2"),SumCoefRange("coeref"));
    //model.fitTo(*data,Range("ref2"),SumCoefRange("ref3"));
    //model.fitTo(*data);
    //RooPlot* frame = x.frame(3,16); 
    RooPlot* frame = x.frame(); 
    data->plotOn(frame);
    model.plotOn(frame,LineColor(kRed),Range("coeref")); 
    model.plotOn(frame,Components(sig2),LineStyle(kDashed),Range("coeref")); 
    model.plotOn(frame,Components(sig),LineStyle(kDashed),Range("coeref"),LineColor(kGreen)); 
    //model.plotOn(frame,Components(sig),LineStyle(kDashed),LineColor(kGreen),Normalization(50000,RooAbsReal::NumEvent)); 
    //model.plotOn(frame,LineColor(kRed),Normalization(1.0,RooAbsReal::RelativeExpected)); 
    //model.plotOn(frame,LineColor(kRed),Normalization(5.27,RooAbsReal::Raw)); 
    //model.plotOn(frame,LineColor(kRed),Normalization(nsig.getVal()+nbkg.getVal(),RooAbsReal::NumEvent)); 
    //model.plotOn(frame,LineColor(kRed),Normalization(267000,RooAbsReal::NumEvent),Range("coeref")); 
    //model.plotOn(frame,LineColor(kRed),Normalization(267000,RooAbsReal::NumEvent)); 
    //model.plotOn(frame,LineColor(kRed),Range("coeref")); 
    //bkg.plotOn(frame); 
    //sig2.plotOn(frame); 
    //sig.plotOn(frame); 
    //model.plotOn(frame,Components(sig),LineStyle(kDashed)); 
    //model.plotOn(frame,Components(bkg)); 
    frame->Draw(); 


}
