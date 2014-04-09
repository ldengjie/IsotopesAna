#ifndef TIMEFITTING_H
#define TIMEFITTING_H

#ifndef __CINT__
#include <RooGlobalFunc.h>
#endif
#include <RooRealVar.h>
#include <RooExponential.h>
#include <RooAddPdf.h>
#include <RooFormulaVar.h>
#include <RooPlot.h>
#include <RooCmdArg.h>
using namespace RooFit;

#include <TH1D.h>
#include <TString.h>
#include <TMath.h>

#include <vector>
#include <iostream>
#include <map>
using namespace std;

class TimeFitting
{
  public:
    TimeFitting();
    ~TimeFitting();

    // User interface
    void DefTau(TString name, const bool fix, const double value, const double low = 0., const double up = 1000.);
    void DefTauMu(TString name, const bool fix, const double value, const double low = 0., const double up = 1000.);
    void AddToken(TString name, TString token);
    //map<TString, RooRealVar*> GetIsoBuf();
    TH1D* GenHist(char* name, bool log, const int nbins = 0);
    void SetLimits(double min, double max);
    double SetUnit(TString unit);// all are relative to 's'
    void SetX();
    RooRealVar* GetX();
    void SetPlot(TString plotName);
    RooAbsPdf* BuildExpPdf(TString pdfName, TString muName, const double constant = 1.0);
    RooAbsPdf* BuildExpBkgPdf(TString pdfName, const double constant = 1.0);
    RooPlot* GetPlot(TString plotName);

  private:
    map<TString, RooRealVar*> m_tau;
    map<TString, TString> m_token; // ex: Li9->^{9}Li
    vector<TString> m_iso;

    RooRealVar* m_x;
    map<TString, RooRealVar*> m_tauMu;
    map<TString, RooPlot*> m_plots;

    TString m_unit;
    double m_tmin;
    double m_tmax;
};

TimeFitting::TimeFitting() {}

TimeFitting::~TimeFitting() {}

RooPlot* TimeFitting::GetPlot(TString plotName) { return m_plots[plotName]; }

void TimeFitting::SetPlot(TString plotName)
{
  TString str;
  vector<TString>::iterator it;
  for(it = m_iso.begin(); it != m_iso.end(); it++){
    str += ", ";
    str += m_token[*it];
  }
  str.Remove(0, 2);
  str += " "; str += plotName;
  m_plots[plotName] = m_x->frame();
  m_plots[plotName]->SetTitle(str.Data());
  //m_plot = m_x->frame(Title(str.Data()));
}

void TimeFitting::SetX() { m_x = new RooRealVar("x", "x", m_tmin, m_tmax, m_unit); }

RooRealVar* TimeFitting::GetX() { return m_x; }

double TimeFitting::SetUnit(TString unit){ 
  m_unit = unit; 
  if(m_unit == "s"){
    return 1.0;
  } 
  else if(m_unit == "ms"){
    return 1000.0;
  }
  else if(m_unit == "#mus"){
    return 1.e6;
  }
  else if(m_unit == "ns"){
    return 1.e9;
  }
  else{
    cout << "Error in TimeFitting.h: Wrong unit-->>" << m_unit << endl ;
    exit(-1);
  }
}

void TimeFitting::SetLimits(double min, double max) { m_tmin = min; m_tmax = max; }

void TimeFitting::AddToken(TString name, TString token) { m_token[name] = token; }

void TimeFitting::DefTau(TString name, const bool fix, const double value, const double low, const double up) 
{ 
  RooRealVar *tau = 0;
  TString isoname = TString("#tau_{") + m_token[name] + TString("}");
  if(fix)
    tau = new RooRealVar(isoname.Data(), isoname.Data(), value, m_unit.Data());
  else
    tau = new RooRealVar(isoname.Data(), isoname.Data(), value, low, up, m_unit.Data());
  m_tau[name] = tau; 
  m_iso.push_back(name);
}

void TimeFitting::DefTauMu(TString name, const bool fix, const double value, const double low, const double up) 
{
  m_tauMu[name] = 0;
  TString isoname = "#tau_{" + m_token[name] + "}";
  if(fix)
    m_tauMu[name] = new RooRealVar(isoname.Data(), isoname.Data(), value, m_unit.Data());
  else
    m_tauMu[name] = new RooRealVar(isoname.Data(), isoname.Data(), value, low, up, m_unit.Data());

}

//map<TString, RooRealVar*> TimeFitting::GetIsoBuf() { return m_tau; }

TH1D* TimeFitting::GenHist(char* name, bool log, const int nbins)
{
  TH1D* hist = 0;
  if(log){
    double bins[1000] = {m_tmin};
    if(nbins > 1000 - 1){
      cout << "Too many bins for log scale!" << endl;
      exit(-1);
    }
    double binWidth = (TMath::Log10(m_tmax)-TMath::Log10(m_tmin))/nbins;
    for(int i = 1; i <= nbins; i++){
      bins[i] = m_tmin*TMath::Power(10, binWidth*i);
    }
    hist = new TH1D(name, name, nbins, bins);
  } else {
    hist = new TH1D(name, name, nbins, m_tmin, m_tmax);
  }
  return hist;
}

RooAbsPdf* TimeFitting::BuildExpPdf(TString pdfName, TString muName, const double constant)
{
  const int nexp = m_tau.size();
  RooFormulaVar* a[nexp];
  RooExponential* exp[nexp];
  TString isoname[nexp];
  RooRealVar* nIsoDay[nexp];
  RooFormulaVar* nIso[nexp];
  RooAddPdf* pdf = 0;
  TString tmp;
  tmp = ""; tmp += constant; tmp += "*@0";

  RooFormulaVar *aMu = new RooFormulaVar("aMu", "aMu", "-1/@0", RooArgList(*(m_tauMu[muName])));
  RooExponential *expMu = new RooExponential("expMu", "expMu", *m_x, *aMu);
  RooRealVar *nMuDay = new RooRealVar("#mu","#mu", 5e4, 0.0, 5.e5);
  RooFormulaVar *nMu = new RooFormulaVar("N_{#mu}","N_{#mu}", tmp.Data(), RooArgList(*nMuDay));

  map<TString, RooRealVar*>::iterator it;
  int i = 0;
  char name[20];
  RooArgSet expSet;
  RooArgSet numSet;
  for(it = m_tau.begin(); it != m_tau.end(); it++){
    sprintf(name, "a%d", i);
    a[i] = new RooFormulaVar(name, name, "-1/@0 + (-1)/@1", RooArgList(*((*it).second), *(m_tauMu[muName])));
    sprintf(name, "exp%d", i);
    exp[i] = new RooExponential(name, name, *m_x, *(a[i]));
    expSet.add(*exp[i]);

    nIsoDay[i] = new RooRealVar(m_token[(*it).first], m_token[(*it).first], 1e4, 0.0, 5.e5);
    isoname[i] = TString("N_{") + m_token[(*it).first] + TString("}");
    nIso[i] = new RooFormulaVar(isoname[i], isoname[i], tmp.Data(), RooArgList(*nIsoDay[i]));
    numSet.add(*nIso[i]);
    i++;
  }
  expSet.add(*expMu);
  numSet.add(*nMu);

  pdf = new RooAddPdf(pdfName.Data(), pdfName.Data(), RooArgList(expSet), RooArgList(numSet));
  //cout << "====== print structure of composite model ======" << endl;
  //cout << "====== printComponentTree()..." << endl;
  //pdf->printComponentTree();
  //cout << "====== printCompactTree()..." << endl;
  //pdf->printCompactTree();
  return pdf;
}

RooAbsPdf* TimeFitting::BuildExpBkgPdf(TString pdfName, const double constant)
{
  const int nexp = m_tau.size();
  RooFormulaVar* a[nexp];
  RooExponential* exp[nexp];
  TString isoname[nexp];
  RooRealVar* nIsoDay[nexp];
  RooFormulaVar* nIso[nexp];
  RooAddPdf* pdf = 0;
  TString tmp;
  tmp = ""; tmp += constant; tmp += "*@0";

  RooRealVar *a0 = new RooRealVar("aZero", "aZero", 0);
  RooExponential *expZero = new RooExponential("expZero", "expZero", *m_x, *a0);
  //RooRealVar *nBkgDay = new RooRealVar("Bkg.", "Bkg.", 5e5, 0, 1.e7);
  RooRealVar *nBkgDay = new RooRealVar("Bkg.", "Bkg.", 5e4, 0, 10e7);
  RooFormulaVar *nBkg = new RooFormulaVar("N_{bkg.}","N_{bkg.}", tmp.Data(), RooArgList(*nBkgDay));

  map<TString, RooRealVar*>::iterator it;
  int i = 0;
  char name[20];
  RooArgSet expSet;
  RooArgSet numSet;
  for(it = m_tau.begin(); it != m_tau.end(); it++){
    sprintf(name, "a%d", i);
    a[i] = new RooFormulaVar(name, name, "-1/@0", RooArgList(*((*it).second)));
    sprintf(name, "exp%d", i);
    exp[i] = new RooExponential(name, name, *m_x, *(a[i]));
    expSet.add(*exp[i]);

    //nIsoDay[i] = new RooRealVar(m_token[(*it).first], m_token[(*it).first], 1e3, 0.0, 5.e5);
    nIsoDay[i] = new RooRealVar(m_token[(*it).first], m_token[(*it).first], 1e4, 0.0, 5.e5);
    isoname[i] = TString("N_{") + m_token[(*it).first] + TString("}");
    nIso[i] = new RooFormulaVar(isoname[i], isoname[i], tmp.Data(), RooArgList(*nIsoDay[i]));
    //nIso[i] = new RooRealVar(isoname[i], isoname[i], 1e4, 0.0, 5.e5);
    numSet.add(*nIso[i]);
    i++;
  }
  expSet.add(*expZero);
  numSet.add(*nBkg);

  pdf = new RooAddPdf(pdfName.Data(), pdfName.Data(), RooArgList(expSet), RooArgList(numSet));
  //RooArgList list = pdf->pdfList();
  //list.Print();
  //list[0].Print();
  //list[1].Print();
  return pdf;
}
#endif //TIMEFITTING_H
