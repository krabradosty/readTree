#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TGraph.h>
#include "TApplication.h"
#include "math.h"

using namespace std;
const Double_t CABIBBO_COS   = 0.974;
const Double_t RAD_CORRECT   = 0.024;
const Double_t G_FERMI       = 1.166 /100000000000. ;
const Double_t SIGMA0        = (G_FERMI*G_FERMI*CABIBBO_COS*CABIBBO_COS)*(1+RAD_CORRECT)/M_PI;
const Double_t DIST          = 3.0856776*pow(10.,22); //см
const Double_t E_NU_TOTAL    = 6.2415096516 * 3.*pow(10. ,58.) /6.; //MeV
const Double_t E_NU_AVERAGE  = 12.; // MeV
const Double_t electron_mass_c2 = 0.510999;
const Double_t neutron_mass_c2 = 939.565378;
const Double_t proton_mass_c2 = 938.272046;

Double_t NuEnFlowSupernova(Double_t enNu);
Double_t MaxwBoltzSpectrum(Double_t enNu);
Double_t IBDSigmaTotal(Double_t enNu);
Double_t IBDPositrinEnergy(Double_t enNu);

int main() {
  TApplication *myapp = new TApplication("myapp",0,0);
  TFile* outputFile = new TFile("/home/krab/out.root", "RECREATE");

  Int_t n = 540;
  Double_t* xPos = new Double_t[n];
  Double_t* y = new Double_t[n];

  Double_t enNu = 1.806;
  Double_t enPos = 0;
  Double_t enStep = 0.1;
  for (int i = 0; i < n; i++){
    xPos[i] = enPos;
    y[i] = NuEnFlowSupernova(enNu) * IBDSigmaTotal(enNu);

    enPos+=enStep;
    enNu+=enStep;
  }

  TGraph* gr = new TGraph(n,xPos,y);
  gr->Write("New");
  outputFile->Write();
  outputFile->Close();

  return 0;
}

Double_t NuEnFlowSupernova(Double_t enNu) {
  return (1./(4*M_PI*pow(DIST, 2.)) * E_NU_TOTAL/E_NU_AVERAGE * MaxwBoltzSpectrum(enNu));
}
Double_t MaxwBoltzSpectrum(Double_t enNu) {
  return (128./3. * pow(enNu, 3)/pow(E_NU_AVERAGE, 4) * exp(-4.*enNu/E_NU_AVERAGE));
}

Double_t IBDSigmaTotal(Double_t enNu) {
  Double_t enPos = IBDPositrinEnergy(enNu);
  return (0.0952*pow(10., -42.) * enPos * sqrt(pow(enPos, 2.) - pow(electron_mass_c2, 2.)));
}

Double_t IBDPositrinEnergy(Double_t enNu) {
  return enNu - (neutron_mass_c2 - proton_mass_c2);
}