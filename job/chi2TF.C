#include <TMinuit.h>
#include <TROOT.h>
#include <TApplication.h>
#include <TInterpreter.h>
#include <TMath.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TF2.h>
#include <TH1D.h>
#include <TVector3.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <map>

using namespace std;
using namespace ROOT;

int verbose = -1;
int nADs = 0;

bool ibd = true;
bool amc = true;
bool spn = true;
bool drift = true;

//bool ibd = false;
//bool amc = false;
//bool spn = false;
//bool drift = false;

double obsTimeIbd[8] = {28.44, 28.15, 28.18, 27.20, 28.17, 27.69, 27.64, 28.35};
double obsTimeSpn[8] = {28.68, 28.61, 28.61, 28.59, 28.59, 28.95, 28.68, 28.39};
double obsTimeAmc[6] = {28.31, 28.18, 28.00, 28.19, 28.14, 28.37};

double errStatIbd[8] = {0.19, 0.18, 0.19, 0.49, 0.50, 0.51, 0.70, 0.28};
double errStatSpn[8] = {0.07, 0.07, 0.08, 0.11, 0.23, 0.24, 0.23, 0.32};
double errStatAmc[6] = {0.18, 0.18, 0.14, 0.17, 0.18, 0.18};

double errSys = 0.00000001;
double TrueTime = 0;

void fcn(int &npar, double *grad, double &fval, double *par, int flag)
{ 
   double chisq = 0;
   int varId = -1;
   vector<double> varNormADi;
   if(ibd) {
       for(int iAD=0; iAD<8; iAD++)
       {
                    varId++;
                    varNormADi.push_back(par[varId]);
       }
   }

   if(drift)  { varId++; double varDriftSpn = par[varId];}

//ibd neutron
   if(ibd) {
       for(int iAD=0; iAD<8; iAD++)
       {
             double nume = obsTimeIbd[iAD]-(TrueTime+varNormADi[iAD]);
             double delta = nume/errStatIbd[iAD];
             chisq+=delta*delta;
    
             delta = varNormADi[iAD]/errSys;
             chisq+=delta*delta;
//cout<<"obs = "<<obsTimeIbd[iAD]<<"true = "<<TrueTime<<" "<<nume<<" "<<errStatIbd[iAD]<<" "<<varNormADi[iAD]<<" "<<errSys<<" "<<chisq<<endl;
       }
   }    
 
//amc neutron
   if(amc) {
       for(int iAD=0; iAD<6; iAD++)
       {
             double nume = obsTimeAmc[iAD]-(TrueTime+varNormADi[iAD]);
             double delta = nume/errStatAmc[iAD];
             chisq+=delta*delta;
    
             delta = varNormADi[iAD]/errSys;
             chisq+=delta*delta;
       }
   }

//spallation neutron
   if(spn) {
       for(int iAD=0; iAD<8; iAD++)
       {
             double nume = obsTimeSpn[iAD]-(TrueTime+varNormADi[iAD]);
     if(drift)  nume = obsTimeSpn[iAD]-(TrueTime+varDriftSpn+varNormADi[iAD]);
//     if(drift)  nume = obsTimeIbd[iAD]-(TrueTime+0.5*(1+varDriftSpn))*(1+varNormADi[iAD]);
             double delta = nume/errStatSpn[iAD];
             chisq+=delta*delta;
    
             delta = varNormADi[iAD]/errSys;
             chisq+=delta*delta;

//     if(drift)     chisq+=varDriftSpn*varDriftSpn;
       }
   }

   fval = chisq;
//cout<<fval<<endl;
}  

//int chi2(int argc, char** argv)
double chi2(double* x,double* p)
{
//        verbose = -1;
//        if(2 == argc){
//                verbose = atoi(argv[1]);
//        }

  TrueTime = x[0];

  if(ibd) nADs += 8;
  if(amc) nADs += 6;
  if(spn) nADs += 8;
  if(drift) nADs += 1;

  TMinuit* dybMinuit;
  dybMinuit = new TMinuit();
  dybMinuit->SetFCN(fcn);

  dybMinuit->SetPrintLevel(verbose);
  double arglist[10];
  int ierrflag = 0;

  dybMinuit->mnexcm("CLEAR", arglist, 0, ierrflag);

  int iPar = -1;
//  dybMinuit->mnparm(iPar, "trueTime", 26., 0.01, 25., 30., ierrflag);

  if(ibd) {
  double errNormADiIbdStart[8] = {-6.28323e-04, -8.17285e-04, 3.74561e-04, 3.15669e-04, 3.34534e-04, 4.20872e-04, 3.34534e-04, 4.20872e-04};
  for(int iAD=0; iAD<8; iAD++)
  {
          iPar++;
          dybMinuit->mnparm(iPar, "errNormADiIbd", errNormADiIbdStart[iAD], 0.01*errSys, 0, 0, ierrflag);
  }
  }

  if(drift) { iPar++; dybMinuit->mnparm(iPar, "errNormDrift", 0, 0.001, 0, 0, ierrflag); }

  arglist[0] = 1;
  dybMinuit->mnexcm("SET STR", arglist, 1, ierrflag);

  arglist[0] = 10000; //maxCalls;
  arglist[1] = 1E-6; //tolerance;
  //  cout<<"good1114 ------"<<endl;
  dybMinuit->mnexcm("MIGrad", arglist, 2, ierrflag);
  //    cout<<"good14 ------"<<endl;

  double min, edm, errdef;
  int nvpar, nparx, icstat;
  dybMinuit->mnstat(min, edm, errdef, nvpar, nparx, icstat);

  double curvalue;
  double curerror;
/*  for(int i=0; i<nADs; i++)
  { 
    dybMinuit->GetParameter(i, curvalue, curerror);
    cout<<"value Par["<<i<<"]  "<<curvalue<<" parerror: "<<curerror<<endl;
  }
*/
  
  cout<<TrueTime<<"  "<<min<<endl;
nADs = 0;
  return min;
}

void chi2TF()
{
        TF1* Chifun = new TF1("Chifun", chi2, 25, 30, 1);
        Chifun->SetNpx(10);
        double bestfit = Chifun->GetMinimumX(27, 29);
        double chi2min = Chifun->Eval(bestfit);
        double right = Chifun->GetX(chi2min+1.0, bestfit, 30);
        double left = Chifun->GetX(chi2min+1.0, 25, bestfit);
        double left3 = Chifun->GetX(chi2min+9, 25, bestfit);
        double right3 = Chifun->GetX(chi2min+9, bestfit, 30);
        double left5 = Chifun->GetX(chi2min+25, 25, bestfit);
        double right5 = Chifun->GetX(chi2min+25, bestfit, 30);
  /*
        double right = Chifun->GetX(chi2min+1.0, bestfit, 33);
        double left = Chifun->GetX(chi2min+1.0, 22, bestfit);
        double left3 = Chifun->GetX(chi2min+9, 22, bestfit);
        double right3 = Chifun->GetX(chi2min+9, bestfit, 33);
        double left5 = Chifun->GetX(chi2min+25, 22, bestfit);
        double right5 = Chifun->GetX(chi2min+25, bestfit, 33);      
*/
        int scan = 100;
        ofstream  chi2file;
        chi2file.open("chi2.dat");
        //cout<<"sin^2(2theta13)  chi2"<<endl;
        for(int i=0; i<scan; i++)
        {
                double time = 25+5.*i/scan;
                chi2file<<time<<" "<<Chifun->Eval(time)<<endl;
        }
        chi2file.close();
        Chifun->Eval(bestfit);

        cout<<"best fit: "<<bestfit<<"+"<<right-bestfit<<"-"<<bestfit-left<<endl;
        cout<<"3sigma: "<<left3<<" "<<right3<<endl;
        cout<<"5sigma: "<<left5<<" "<<right5<<endl;
        cout<<"chi2min: "<<chi2min<<endl;
}
