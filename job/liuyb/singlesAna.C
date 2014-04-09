#include "TimeFitting.h"
//#include "calcTime.h"
//#include "setHistEnergyYaxisTitle.h"
#include <TTree.h>
#include <TLegend.h>
#include <TPad.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TLegendEntry.h>
#include <TH2D.h>
#include <TStyle.h>
#include <RooDataHist.h>
#include <iostream>
#include <utility>
using namespace std;

const TString dataset  = "P13A";
const TString fileDir  = "/publicfs/dyb/data/userdata/liuyb/LAF/SinglesAlg/run/P13A";
const TString site     = "EH1";
      int     displayAdNum = 2;
const int     eMuonMinArr[] = {20, 60, 500, 1500, 2500, 3500, 4500, 5000};
const int     kSlice   = 6;

const bool    debug = 0;
const int     EntriesToBeProcessed = -1;
const bool    loop = 1;

const bool    doFit = 1;
const TString fitType = "B12"; const bool logx = 0; const bool logy = 0;
const int     Nbins = 499;
const bool    ParamOn = 1;
const TString Unit = "ms";
const bool    useConstant = 0;

// Select which hists to be drawn
const bool    drawMuon = 0; const double muonBinWidth = 10; // MeV, for rebinning muon energy vs. neutron multiplicity
const bool    drawSpec = 0;
const bool    drawSpaceDis = 0;
const bool    drawSingleRate = 0;

bool eCut = 1;            // energy cut
bool flasherCut = 0;      // additional flasher cut
bool vertexCut = 0;       // vertex cut 
bool multiplicityCut = 1; // muon multiplicity cut

const double  eBinWidth = 0.25; // MeV per bin

const double  tminB12Sig       = 0.001; const double tmaxB12Sig = 0.1;
const double  tminB12Bkg       = 0.301; const double tmaxB12Bkg = 0.4;

const bool    fixMu  = 1;
const double  eSingleMin = 5.5; // energy range for singles
const double  eSingleMax = 20.;
const double  ePositronMin = 1.8;
const double  ePositronMax = 3.0;

const TString isotopes[] = {"B12", "N12", "C9", "Li8", "B8", "He8", "Li9", "Be11", "e+", "single"};
const int     IsoNum = sizeof(isotopes)/sizeof(TString);
const int     color[] = {kBlue, kRed, kBlack};
const int     markerstyle[] = {20, 24, 21};

TString fileName;
TString plotName;
double tmin;
double tmax;
int    ADNUM;
double isolatingCutEff[4];
map<TString, double> tauVal;
map<TString, double> tauMin;
map<TString, double> tauMax;
map<TString, bool>   fixTau;
map<TString, pair<double, double> > limits;
vector<pair<TString, double> > muonRate;
vector<pair<TString, double> > muonRateAftRed;
int DaqTime = 0;
int LiveTime[4] = {0};
double constant[4];
int vertexInCenter[4] = {0};// Record events reconstructed to -0.1<Z<0 to put an error on ACU neutrons

void     doFitting(TVirtualPad*, RooPlot*, RooAbsPdf*, TH1D*, const int, bool);
void     drawLegend();
void     drawAdLegend();
//void     drawAdLegend(double x1 = 0.7, double y1 = 0.8, double x2 = 0.9, double y2 = 0.9);
TLegend* drawLegend(double* pos, TString str1, TString opt1 = "", TString str2 = "", TString opt2 = "", TString str3 = "", TString opt3 = "");
void     setLimits(TString);
TString  str2strForIso(TString str);
void     setTauValues();
TH1D*    initHist(TH1D*, char*);
void     getMuonRate(double, TH1D**, TH1D**);
vector<double> getMuonRate(const int, TH1D**, const double, double);
void     livetime();
void     getTrigRate(TH1D**);
void     calcMuonNum(TH1D**);
void     getDiff(TH2D**);
void     fitSingle(TH1D**);
TCanvas* draw3TH1(TH1D* h1, TH1D* h2, TH1D* h3, TString cName, 
                  TString xTitle, TString yTitle = "", 
                  const TString opt1 = "", const TString opt2 = "", const TString opt3 = "", 
                  bool logy = true, int w = 800, int h = 600);
TCanvas* drawTH2(TH2D* h, TString cName, 
                  TString xTitle, TString yTitle, TString option = "colz");
void     calcAmCBkg(TH1D** h, TH1D** h_Zgt0, TH1D** h_Zlt0);
TH1D*    reBinLiveTime(TH1D* h, char* histName, const int reBinWidth);
TH2D*    getMuonEnergyVsNeuMulti(TH1D* h_eMuon, TH2D* h_eMuonVsNeuMulti, double binWidth);

TH1D* h_livetime[4];
TH1D* h_reBinnedLiveTime[4];
TH1D* h_daqtime; 
const int timeBinWidth = 24;
TimeFitting *timeFitting;
char name[100];
double pos[4]; // for TLegend position
TCanvas* c = 0;
TString xTitle, yTitle;
TLegend* legend = 0;
TString cName;
const int ArrayLen  = 1000;
char timeFormat[] = "%b %d"; //"#splitline{%H:%M}{%b%d}"; "#splitline{%b%d}{%Y}"; "#splitline{%H:%M:%S}{%b %d %Y}";
double ibdUpOverLow[4];
double ibdUpOverLowErr[4];
int singlesAna()
{

  if(site == "EH1"){
    ADNUM = 1;
    ibdUpOverLow[0]    = .970; ibdUpOverLow[1]    = .960;
    ibdUpOverLowErr[0] = .006; ibdUpOverLowErr[1] = .006;
    //ibdUpOverLow[0]    = .967; ibdUpOverLow[1]    = .946;
    //ibdUpOverLowErr[0] = .008; ibdUpOverLowErr[1] = .008;
    //ibdUpOverLow[0]    = .947; ibdUpOverLow[1]    = .951;
    //ibdUpOverLowErr[0] = .017; ibdUpOverLowErr[1] = .017;
  }
  else if(site == "EH2"){
    ADNUM = 1;
    ibdUpOverLow[0]    = .947;
    ibdUpOverLowErr[0] = .007;
    //ibdUpOverLow[0]    = .952;
    //ibdUpOverLowErr[0] = .008;
    //ibdUpOverLow[0]    = .939;
    //ibdUpOverLowErr[0] = .019;
  }
  else if(site == "EH3"){
    ADNUM = 3;
    ibdUpOverLow[0]    = .95; ibdUpOverLow[1]    = 1.03; ibdUpOverLow[2]    = .98;
    ibdUpOverLowErr[0] = .02; ibdUpOverLowErr[1] = .02;  ibdUpOverLowErr[2] = .02;
    //ibdUpOverLow[0]    = .92; ibdUpOverLow[1]    = 1.00; ibdUpOverLow[2]    = .98;
    //ibdUpOverLowErr[0] = .02; ibdUpOverLowErr[1] = .02;  ibdUpOverLowErr[2] = .02;
    //ibdUpOverLow[0]    = .899; ibdUpOverLow[1]    = .973; ibdUpOverLow[2]    = .902;
    //ibdUpOverLowErr[0] = .046; ibdUpOverLowErr[1] = .050; ibdUpOverLowErr[2] = .048;
  }
  else{
    cerr << "Error: Unknown site name " << site << endl;
    exit(-1);
  }
  if(displayAdNum >= ADNUM) displayAdNum = ADNUM;

  //int count[4] = {0};
  const double eMuonLow  = eMuonMinArr[kSlice];// eMuonLow and eMuonHigh should be consistent with 
  const double eMuonHigh = eMuonMinArr[kSlice + 1];

  cout << "------------------------------" << endl ;
  cout << "Input file:\n" << fileName << endl ;
  TFile *f = new TFile("/publicfs/dyb/data/userdata/liuyb/LAF/SinglesAlg/run/P13A/Selected_SinglesAlg_EH1_P13A_Muon4500.root");
  h_daqtime = (TH1D*)f->Get("LiveTime/DaqTime");
  TH1D* h_muonSpec[4] = {0};
  TH1D* h_muonSpecAftRed[4] = {0};
  //for(int i = 0; i < ADNUM; i++){
  for(int i = 0; i < 1; i++){
      //sprintf(name, "liuyb/MuonSpec_AD%d", i+1);
      //h_muonSpec[i] = (TH1D*)f->Get(name);
      h_muonSpec[i] = (TH1D*)f->Get("liuyb/MuonSpec_AD1");
    //sprintf(name, "liuyb/MuonSpecAfterReduction_AD%d", i+1);
    //h_muonSpecAftRed[i] = (TH1D*)f->Get(name);
    h_muonSpecAftRed[i] = (TH1D*)f->Get("liuyb/MuonSpecAfterReduction_AD1");
  }
  
  livetime();
  std::cout<<"2 "<<endl;
  vector<double> realTauMu;
  vector<double> realTauMuAftRed;
  realTauMu = getMuonRate(DaqTime, h_muonSpec, eMuonLow, eMuonHigh);
  realTauMuAftRed = getMuonRate(DaqTime, h_muonSpecAftRed, eMuonLow, eMuonHigh);

  //TFile *f1 = new TFile("/afs/ihep.ac.cn/users/l/lidj/file/IsotopesAna/job/EH1totalHisto_P13A.root");
  //TH1D* h1=(TH1D*)f1->Get("lidj/time2lastshowermuon1");
  //TH1D* h= new TH1D("h","h",499,1,500);
  //for( int i=0 ; i<500 ; i++ )
  //{
  //h->SetBinContent(i+1,h1->GetBinContent(i+2));
  //}

  TFile *f1 = new TFile("/afs/ihep.ac.cn/users/l/lidj/file/IsotopesAna/job/liuyb/7slice_liuyb.root");
  TH1D* h1=(TH1D*)f1->Get("EH1/Muon20to60/Time2LastMuon_noReduction_ads_Muon20to60");
  TH1D* h= new TH1D("h","h",499,1,500);
  for( int i=0 ; i<500 ; i++ )
  {
      h->SetBinContent(i+1,h1->GetBinContent(i+1));
  }
  

  setLimits(fitType);
  timeFitting = new TimeFitting();
  const double unitCo = timeFitting->SetUnit(Unit);
  timeFitting->SetLimits(tmin*unitCo, tmax*unitCo);
  timeFitting->SetX();

  setTauValues();
  for(int i = 0; i < IsoNum; i++){
    if(fitType.Contains(isotopes[i])){
      TString tmp;
      if(isotopes[i].IsAlnum()) {
        tmp = str2strForIso(isotopes[i]);
      }
      if(isotopes[i] == "single") tmp = "single";
      if(isotopes[i] == "e+") tmp = "e^{+}";
      timeFitting->AddToken(isotopes[i], tmp);
      timeFitting->DefTau(isotopes[i], 
          fixTau[isotopes[i]], 
          tauVal[isotopes[i]]*unitCo, 
          tauMin[isotopes[i]]*unitCo, 
          tauMax[isotopes[i]]*unitCo);
    }
  }
  for(int i = 0; i < ADNUM; i++){
    sprintf(name, "#mu_{b.e.%d}", i+1);
    muonRate.push_back(pair<TString, double>(TString(name), realTauMu[i]));
    timeFitting->AddToken(name, name);
    timeFitting->DefTauMu(name, fixMu, 
        muonRate[i].second*unitCo, 0*unitCo, 100*unitCo);
    sprintf(name, "#mu_{a.e.%d}", i+1);
    muonRateAftRed.push_back(pair<TString, double>(TString(name), realTauMuAftRed[i]));
    timeFitting->AddToken(name, name);
    timeFitting->DefTauMu(name, fixMu, 
        muonRateAftRed[i].second*unitCo, 0*unitCo, 100*unitCo);
  }

  if(fitType == "single"){
  } 
  else if(fitType == "e+"){
  }
  else {
    TCanvas* cFit = new TCanvas("cFit", "cFit", 1200, 800);
    cFit->Divide(2,2);

    plotName = "fit to last muon";
    timeFitting->SetPlot(plotName);
    RooPlot* frame1 = timeFitting->GetPlot(plotName);
    RooAbsPdf* func1; 
    for(int i = 0; i < 1; i++){
      double coeff = (useConstant == true)? constant[i]:1.0;
      sprintf(name, "expPdfNoRed_AD%d", i+1);
      func1 = timeFitting->BuildExpPdf(name, muonRateAftRed[i].first, coeff);
      //doFitting(cFit->GetPad(1), frame1, func1[i], h_t2last[i], i, logx);
      doFitting(cFit->GetPad(1), frame1, func1, h, i, logx);
    }
      func1->Print("t");
    frame1->GetXaxis()->SetTitle("Time since last muon [" + Unit + "]");
    frame1->SetTitle("");

  }


  return 0;
}

void doFitting(TVirtualPad* c, RooPlot* xframe, RooAbsPdf* func, TH1D* h, const int det, bool logx)
{
  RooRealVar* x = timeFitting->GetX();
  c->cd();
  RooDataHist* data;
  sprintf(name, TString(TString("data_") + h->GetName()));
  data = new RooDataHist(name, name, *x, h);
  func->fitTo(*(data));
  data->plotOn(xframe, LineStyle(1), LineWidth(1), MarkerSize(0.5), MarkerStyle(2));
  //data->plotOn(xframe, LineStyle(1), LineWidth(1), MarkerColor(color[det]), MarkerSize(0.5), MarkerStyle(2));
  func->plotOn(xframe, LineColor(color[det]));
  if(ParamOn){
    //double minx = 0.35, maxx = 0.55; double span = 0.25, maxy = 0.90;
    double minx = 0.15, maxx = 0.35; double span = 0.25, maxy = 0.90;
 
    //double minx = 0.15, maxx = 0.35; double span = 0.30, maxy = 0.90;
    //double minx = 0.45, maxx = 0.55; double span = 0.30, maxy = 0.90;
    func->paramOn(xframe, Layout(minx + det*span, maxx + det*span, maxy), Label(""),Format("NEU",AutoPrecision(1)));
    xframe->getAttText()->SetTextColor(color[det]);
    if(site == "EH3") xframe->getAttText()->SetTextSize(0.04);
    xframe->getAttFill()->SetFillStyle(0);
    xframe->getAttLine()->SetLineColor(0);
  }
  if(logy) c->SetLogy();
  if(logx) c->SetLogx();
  //double chi2 = xframe->chiSquare();
  xframe->Draw();
  xframe->GetXaxis()->SetNdivisions(505);
  double binWidth;
  binWidth = h->GetBinWidth(1);
  sprintf(name, "Entries / %.2f%s", binWidth, Unit.Data());
  xframe->GetYaxis()->SetTitle(name);
  //double chi2 = xframe->chiSquare(func->GetName(), h->GetName(), 0);
  //cout << "\n----------------------------------------------------" << endl ;
  //cout << "chi2 = " << chi2 << endl;
  //cout << "funcName=" << func->GetName()
  //  << ", histName=" << h->GetName() << endl ;
  //cout << "\n----------------------------------------------------" << endl ;
  drawLegend();
}

void drawAdLegend()
//void drawAdLegend(double x1, double y1, double x2, double y2)
{
  double pos[4];
  //pos[0] = x1; pos[1] = y1; pos[2] = x2; pos[3] = y2;
  if(site == "EH1"){
    pos[0] = 0.7; pos[1] = 0.78; pos[2] = 0.9; pos[3] = 0.9;
    drawLegend(pos, "AD1", "lp", "AD2", "lp");
  }
  else if(site == "EH2"){
    pos[0] = 0.7; pos[1] = 0.84; pos[2] = 0.9; pos[3] = 0.9;
    drawLegend(pos, "AD3", "lp");
  }
  else if(site == "EH3"){
    pos[0] = 0.7; pos[1] = 0.72; pos[2] = 0.9; pos[3] = 0.9;
    drawLegend(pos, "AD4", "lp", "AD5", "lp", "AD6", "lp");
  }

}

void drawLegend()
{
  TLegend *leg = new TLegend(0.7, 0.9-0.06*displayAdNum, 0.9, 0.9);
  //TLegend *leg = new TLegend(0.2, 0.3, 0.4, 0.5);
  TLegendEntry *entry = 0;
  for(int i = 0; i < displayAdNum; i++){
    //entry = leg->AddEntry(ad[i].Data(), ad[i].Data(), "");
    sprintf(name, TString(site + "-AD%d").Data(), i+1);
    entry = leg->AddEntry(name, name, "");
    entry->SetTextColor(color[i]);
  }
  leg->Draw();
  leg->SetFillStyle(0);
}

TString str2strForIso(TString str)
{
  int len = str.Length();
  int mass = 0;
  for(int i = 1; i <= len; i++){
    if(TString(str[len-i]).IsAlpha()){
      mass = len - i;
      break;
    }
  }

  TString s = "^{" + str(mass+1, len-mass) + "}" + str(0, mass+1);
  return s;
}

TH1D* initHist(TH1D* h_mother, char *histName)
{
  if(!h_mother){
    cout << "Failed to initialize " << histName << endl;
    exit(-1);
  }

  TString str = histName;
  Ssiz_t pos = str.Last('/');
  str = str(pos+1, str.Length() - pos - 1);
  TH1D* h = new TH1D(*h_mother);
  h->SetName(str.Data());
  h->SetTitle(str.Data());
  h->Reset();
  return h;
}

TH2D* getMuonEnergyVsNeuMulti(TH1D* h_eMuon, TH2D* h_eMuonVsNeuMulti, double binWidth)
{
  int bins1D = h_eMuon->GetNbinsX();
  int bins2D = h_eMuonVsNeuMulti->GetNbinsX();
  if(bins1D != bins2D){
    cout << "Error: Different bin numbers! " << bins1D << "!=" << bins2D << endl;
    exit(-1);
  }

  //double w = h_eMuon->GetBinWidth(1);
  double up = h_eMuon->GetBinLowEdge(bins1D + 1);
  int    bins = int(up*1000./binWidth);
  int    binsY2D = 50;
  TH2D* h_eMuonNormVsNeuMulti = new TH2D("h", "h", bins, 0, up, binsY2D, 0, binsY2D);
  for(int i = 1; i <= bins2D; i++){
    for(int j = 1; j <= binsY2D; j++){
      h_eMuonNormVsNeuMulti->Fill((i - 1)/1000., j - 1, h_eMuonVsNeuMulti->GetBinContent(i,j));
    }
  }
  TH1D* h_tmp = new TH1D(*h_eMuon);
  h_tmp->SetName("");
  h_tmp->SetTitle("");
  h_tmp->Rebin(int(binWidth));
  for(int i = 1; i <= bins; i++){
    if(h_tmp->GetBinContent(i) > 0){
      for(int j = 1; j <= binsY2D; j++){
        double val = h_eMuonNormVsNeuMulti->GetBinContent(i, j);
        h_eMuonNormVsNeuMulti->SetBinContent(i, j, val*1./h_tmp->GetBinContent(i));
      }
    }
  }

  return h_eMuonNormVsNeuMulti;
}


vector<double> getMuonRate(const int daqtime, TH1D** h, const double low, double up)
{
  cout << "\n....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo...." << endl;
  if(daqtime <= 0){
    cout << "Wrong DAQ time, which is " << daqtime << endl ;
    exit(-1);
  }

  vector<double> tau;
  for(int i = 0; i < ADNUM; i++){
    int bin1 = h[i]->FindBin(low/1000.);
    int bin2 = h[i]->GetNbinsX();
    if(up > 0){
      bin2 = h[i]->FindBin(up/1000.);
    } else {
      up = h[i]->GetBinLowEdge(bin2 + 1)*1000.;
    }
    double sum = h[i]->Integral(bin1, bin2);
    tau.push_back(daqtime*1./sum);
    cout << "Muon rate in range [" << low 
      << "," << up 
      << "] of AD" << i+1 
      << " is " << 1./tau[i]
      << " Hz, muon mean lifetime is " 
      << tau[i] << " s."
      << endl ;
  }

  return tau;
}

void getMuonRate(double daqtime, TH1D** h_spec, TH1D** h_rate)
{
  if(daqtime <= 0){
    cout << "Wrong DAQ time, which is " << daqtime << endl ;
    return;
  }

  for(int i = 0; i < ADNUM; i++){
    int nbin = h_spec[i]->GetNbinsX();
    for(int bin = 1; bin <= nbin; bin++){
      double cnt = h_spec[i]->Integral(bin, nbin);
      double rate = cnt*1./daqtime;
      h_rate[i]->SetBinContent(bin, rate);
    }
  }
}

void livetime()
{
  if(!h_daqtime){
    cout << "Failed to get LiveTime/DaqTime" << endl ;
    exit(-1);
  } else {
    DaqTime = int(h_daqtime->Integral());
    cout << "DaqTime of " << site << " is " << DaqTime 
      << "(" << DaqTime/86400.
      << " days)" << endl ;
  }
  
  
  //for(int i = 0; i < ADNUM; i++){
  //if(!h_livetime[i]) {
  //cout << "Failed to get " << name << endl ;
  //exit(-1);
  //} else {
  //LiveTime[i] = int(h_livetime[i]->Integral());
  //cout << "Livetime for AD" << i+1 << " is " << LiveTime[i] 
  //<< "(" << LiveTime[i]*1./86400
  //<< " days)" << endl ;
  //}
  //}
}

TH1D* reBinLiveTime(TH1D* h, char* histName, const int reBinWidth)
{
  if(!h){
    cout << "Failed to get original livetime hist!" << endl ;
    exit(-1);
  }

  double binWidth = h->GetBinWidth(1);
  int nBins = h->GetNbinsX();
  cout << "Info: original time bin width is " << binWidth/3600
    << ", rebinning width is " << reBinWidth
    << endl;
  int times = int(reBinWidth*3600/binWidth);
  TH1D* h_reBin = new TH1D(histName, histName, 
      nBins/times, h->GetBinLowEdge(1), h->GetBinLowEdge(nBins+1));
  for(int i = 1; i <= nBins; i++){
    double val = h->GetBinContent(i);
    double low = h->GetBinLowEdge(i);
    h_reBin->Fill(low, val);
  }

  return h_reBin;
}

void getTrigRate(TH1D** h)
{
  cout << "\n....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo...." << endl;
  for(int i = 0; i < ADNUM; i++){
    if(!h[i]){
      cout << "Failed to get TriggerSepc_afterMuonVeto_AD" << i+1 << endl;
      exit(-1);
    }
    double integral = h[i]->Integral();
    if(LiveTime[i] > 0.){
      double rate = integral/LiveTime[i];
      isolatingCutEff[i] = TMath::Exp(-2*200*1.e-6*rate);
      cout << "Trigger rate(>0.7MeV) in " << site << "-AD" << i+1 << " is " 
        << rate << " Hz" 
        << ", isolating cut efficiency = " << isolatingCutEff[i]
        << endl;
    } else {
      cout << "Livetime of " << site << "-AD" << i+1 << " is " << "0." << endl;
      exit(-1);
    }
  }
}

void calcAmCBkg(TH1D** h, TH1D** h_Zgt0, TH1D** h_Zlt0)
{
  for(int i = 0; i < ADNUM; i++){
    double bkg = h[i]->Integral();
    double rate;
    if(constant[i] > 0) {
      rate = bkg/constant[i];
    } 
    else {
      cout << "No correction constant for AD" << i+1 << endl ;
      exit(-1);
    }
    double sum_gt0 = h_Zgt0[i]->Integral();
    double sum_lt0 = h_Zlt0[i]->Integral();
    cout << "Singles number (Z>0): " << sum_gt0 << ", singles number (Z<0): " << sum_lt0 << endl;
    double bkgError = sqrt(sum_gt0 + pow(ibdUpOverLow[i]*sum_lt0, 2)*(pow(ibdUpOverLowErr[i]/ibdUpOverLow[i], 2) + 1./sum_lt0));
    //double bkgError = sqrt(h_Zgt0[i]->Integral() + h_Zlt0[i]->Integral());
    //double bkgError = vertexInCenter[i];
    double rateError = bkgError/constant[i];
    cout << "Background from AmC: "
      << "Det=" << i+1
      << ", bkg statistics=" << bkg << "+-" << bkgError
      << ", rate=" << rate << "+-" << rateError
      << endl;
  }
}

void calcMuonNum(TH1D** h)
{ 
  cout << "\n....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo...." << endl;
  cout << "Use the live time of AD1 to calculate the muon rate(/day/AD)." << endl ;
  if(h[0]->GetBinWidth(1) != 0.001){
    cout << "Bin width of " << h[0]->GetName() << " is not 1MeV!" << endl ;
    exit(-1);
  }
  const int slices = sizeof(eMuonMinArr)/sizeof(int);
  //const int slices = 5
  double sliceNum[slices] = {0};
  double sliceRate[slices] = {0};
  double sliceRateErr[slices] = {0};
  for(int i = 0; i < slices; i++){
    for(int det = 0; det < ADNUM; det++){
      sliceNum[i] += h[det]->Integral(eMuonMinArr[i]+1, eMuonMinArr[i+1]);
    }
    sliceNum[i] = sliceNum[i]*1./ADNUM;
    sliceRate[i] = sliceNum[i]*1./LiveTime[0];
    sliceRateErr[i] = sqrt(sliceNum[i])/LiveTime[0];
    cout << "slice " << eMuonMinArr[i] << "\t:\t" << sliceRate[i]
      << "+-" << sliceRateErr[i]
      << ", /day: " << sliceRate[i]*86400
      << "+-" << sliceRateErr[i]*86400
      << endl;
  }

  double rate[4];
  double rateErr[4];
  for(int i = 0; i < ADNUM; i++){
    rate[i] = h[i]->Integral()*1./LiveTime[i];
    rateErr[i] = sqrt(h[i]->Integral())*1./LiveTime[i];
    cout << "Muon rate in AD" << i+1
      << " is " << rate[i]
      << "+-" << rateErr[i]
      << endl ;
  }
  //double num = 0;
  //double rate;
  //double rateErr;
  //for(int i = 0; i < ADNUM; i++){
  //  num += h[i]->Integral();
  //  cout << "Muon rate in AD" << i+1
  //    << " is " << h[i]->Integral()/LiveTime[i]
  //    << endl ;
  //}
  //num = num*1./ADNUM;
  //rate = num/LiveTime[0];
  //rateErr = sqrt(num)/LiveTime[0];
  //cout << "Average muon rate = " << rate
  //  << " +- " << rateErr
  //  << ", /day: " << rate*86400
  //  << "+- " << rateErr*86400
  //  << endl;
}

TCanvas* draw3TH1(TH1D* h1, TH1D* h2, TH1D* h3, TString cName, TString xTitle, TString yTitle, const TString opt1, const TString opt2, const TString opt3, bool logy, int w, int h)
{
  const TString str = h1->GetName();
  TString option;
  TCanvas *c = new TCanvas(cName.Data(), cName.Data(), w, h);
  //TCanvas *c = new TCanvas(cName.Data(), cName.Data(), 400, 300);
  c->cd();
  if(logy) c->SetLogy();
  if(h1){
    h1->GetXaxis()->SetTitle(xTitle);
    if(yTitle != "") h1->GetYaxis()->SetTitle(yTitle);
    h1->SetLineColor(color[0]);
    h1->SetMarkerStyle(markerstyle[0]);
    h1->SetMarkerColor(color[0]);
    h1->SetMarkerSize(1);
    h1->SetTitle("");
    h1->SetStats(kFALSE);
    //gStyle->SetOptStat(1111111);
    option = opt1;
    h1->DrawCopy(option);
    h1->GetXaxis()->SetNdivisions(-505);
  }

    if(h2){
      h2->SetLineColor(color[1]);
      h2->SetMarkerStyle(markerstyle[1]);
      h2->SetMarkerColor(color[1]);
      option = opt2;
      option += "same";
      h2->DrawCopy(option);
    }

    if(h3){
      h3->SetLineColor(color[2]);
      h3->SetMarkerStyle(markerstyle[2]);
      h3->SetMarkerColor(color[2]);
      option = opt3;
      option += "same";
      h3->DrawCopy(option);
    }
  return c;
}

void getDiff(TH2D** h)
{
  const int nbinsX = h[0]->GetNbinsX();
  const int nbinsY = h[0]->GetNbinsY();
  for(int det = 0; det < ADNUM; det++){
    int numUp = 0; int numLow = 0;
    for(int i = 1; i <= nbinsX; i++){
      for(int j = 1; j <= nbinsY; j++){
        if(j <= nbinsY/2) numLow += int(h[det]->GetBinContent(i, j));
        else numUp += int(h[det]->GetBinContent(i, j));
      }
    }
    cout << "Number of events for Z>0: " << numUp
      << ", number of events for Z<0: " << numLow
      << ", number difference: " << numUp-numLow
      << endl ;
  }
}

TLegend* drawLegend(double* pos, TString str1, TString opt1, TString str2, TString opt2, TString str3, TString opt3)
{
  std::vector<TString> str;
  if(str1 != "") str.push_back(str1);
  if(str2 != "") str.push_back(str2);
  if(str3 != "") str.push_back(str3);
  std::vector<TString> opt;
  opt.push_back(opt1);
  opt.push_back(opt2);
  opt.push_back(opt3);
  //int size = str.size();

  TLegend *leg = new TLegend(pos[0], pos[1], pos[2], pos[3]);
  TLegendEntry *entry = 0;
  for(unsigned int i = 0; i < str.size(); i++){
    int iColor = color[i];
    //int iColor = size > 1 ? color[i] : 1;
    TH1D* h = new TH1D();
    h->SetLineColor(iColor);
    h->SetMarkerColor(iColor);
    h->SetMarkerStyle(markerstyle[i]);
    entry = leg->AddEntry(h, str[i], opt[i]);
    entry->SetTextColor(iColor);
    entry->SetLineColor(iColor);
  }
  leg->Draw();
  leg->SetMargin(0.25);
  leg->SetEntrySeparation(0.05);
  leg->SetFillStyle(0);
  return leg;
}

TCanvas* drawTH2(TH2D* h, TString cName, TString xTitle, TString yTitle, TString option)
{
  TCanvas *c = new TCanvas(cName.Data(), cName.Data(), 600, 400);
  c->cd();
  if(h){
    h->GetXaxis()->SetTitle(xTitle);
    h->GetXaxis()->SetNdivisions(505);
    h->GetYaxis()->SetTitle(yTitle);
    h->GetYaxis()->SetNdivisions(505);
    h->SetTitle("");
    h->DrawCopy(option);
  }
  return c;
}

void fitSingle(TH1D** h)
{
  if(!h){
    cout << "Warn: Failed to get hist for fitSingle" << endl;
  }
  // For fitting singles 
  TCanvas *cPos = new TCanvas("cSingle", "cSingle", 1200, 400);
  cPos->Divide(2,1);

  TimeFitting* tFitting = new TimeFitting();
  const double unitCo = tFitting->SetUnit(Unit);
  tFitting->SetLimits(0.0*unitCo, .5*unitCo);
  tFitting->SetX();
  tFitting->AddToken("p1", "p1");
  tFitting->DefTau("p1", 0, 0.030*unitCo, 0.001*unitCo, 0.1*unitCo);
  tFitting->AddToken("p2", "p2");
  tFitting->DefTau("p2", 0, 0.03*unitCo, 0.001*unitCo, 5.1*unitCo);
  tFitting->AddToken("p3", "p3");
  tFitting->DefTauMu("p3", 0, 1000*unitCo, 1*unitCo, 1.e8*unitCo);

  // Common fitting function
  RooAbsPdf* func[4];
  for(int i = 0; i < ADNUM; i++){
    double coeff = 1.0;
    //double coeff = (useConstant == true)? constant[i]:1.0;
    sprintf(name, "expBkgPdf_AD%d", i+1);
    //func[i] = tFitting->BuildExpBkgPdf("expBkgPdf", coeff);
    func[i] = tFitting->BuildExpPdf(name, "#mutest", coeff);
  }

  //// 1
  //plotName = "fit ";
  //timeFitting->SetPlot(plotName);
  //RooPlot* frame5 = timeFitting->GetPlot(plotName);
  //for(int i = 0; i < displayAdNum; i++){
  //  doFitting(cPos->GetPad(1), frame5, func[i], h[i], i, logx);
  //}
  //frame5->GetXaxis()->SetTitle("Time since last single [" + Unit + "]");
}

void setLimits(TString fit)
{
  limits["B12"]            = pair<double, double>(0.001, 0.5);
  limits["N12"]            = pair<double, double>(0.001, 0.5);
  limits["B12N12"]         = pair<double, double>(0.001, 0.5);
  limits["C9"]             = pair<double, double>(0.150, 2.0);
  limits["B12C9He8Li9Li8"] = pair<double, double>(0.001, 2.0);
  limits["C9Li8"]          = pair<double, double>(0.150, 10.0);
  limits["C9Li8B8"]        = pair<double, double>(0.150, 20.0);
  limits["Li8B8"]          = pair<double, double>(0.8, 10.0);
  limits["Li8"]            = pair<double, double>(0.8, 10.0);
  limits["B12C9Li8"]       = pair<double, double>(0.001, 20.0);
  limits["B12N12C9Li8"]    = pair<double, double>(0.001, 20.0);
  limits["B12C9Li9Li8"]    = pair<double, double>(0.001, 20.0);
  limits["e+"]             = pair<double, double>(0.0002, 0.002);
  limits["single"]         = pair<double, double>(0.00, 20);

  map<TString, pair<double, double> >::iterator it;
  it = limits.find(fit);
  if(it != limits.end()){
    tmin = limits[fit].first; tmax = limits[fit].second;
  } else {
    cout << "No fitting command: " << fit << endl ;
    exit(-1);
  }
}

void setTauValues()
{
  fixTau["B12"] = 1; tauVal["B12"] = 0.0291; tauMin["B12"] = 0.011; tauMax["B12"] = 0.8;
  fixTau["N12"] = 0; tauVal["N12"] = 0.0159; tauMin["N12"] = 0.005; tauMax["N12"] = 0.08;
  fixTau["C9"]  = 1; tauVal["C9"]  = 0.1825; tauMin["C9"]  = 0.051; tauMax["C9"]  = 0.5;
  fixTau["Li8"] = 1; tauVal["Li8"] = 1.2100; tauMin["Li8"] = 0.511; tauMax["Li8"] = 5.0;
  fixTau["B8"]  = 0; tauVal["B8"]  = 1.1100; tauMin["B8"]  = 0.511; tauMax["B8"]  = 5.0;
  fixTau["He8"] = 0; tauVal["He8"] = 0.1717; tauMin["He8"] = 0.081; tauMax["He8"] = 0.8;
  fixTau["Li9"] = 0; tauVal["Li9"] = 0.2572; tauMin["Li9"] = 0.111; tauMax["Li9"] = 0.8;
  fixTau["Be11"]= 0; tauVal["Be11"]= 19.900; tauMin["Be11"]= 5.011; tauMax["Be11"]= 50.;
  fixTau["e+"]  = 0; tauVal["e+"]  = 207e-6; tauMin["e+"]  = 1.e-5; tauMax["e+"]  = 0.001;
  //fixTau["single"]=0;tauVal["single"]=0.030; tauMin["single"]=0.001;tauMax["single"]=0.1;
}

