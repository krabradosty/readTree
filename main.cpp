#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include "TApplication.h"

using namespace std;

int main() {
  TApplication *myapp = new TApplication("myapp",0,0);
  Double_t mupeTime[200000];
  Int_t muNpe;


  TFile* inputFile = new TFile("/home/krab/workdir/simulation/supernova_80PMTs.root");
  TTree* tree =(TTree*) inputFile->Get("dstree");
  TFile* outputFile = new TFile("/home/krab/workdir/simulation/readTree/out.root", "RECREATE");
  TH1F* hMuTime = new TH1F("hPeTime", "Time distribution of photoelectron registration;t_{pe} [ns];number", 200, 0, 800);

  tree->SetBranchAddress("munpe", &muNpe);
  tree->SetBranchAddress("mupe_time", mupeTime);
  auto nentries = tree->GetEntries();
  for (long i = 0; i < nentries; i++) {
    tree->GetEntry(i);
    for (int j=0; j < muNpe; j++) {
      hMuTime->Fill(mupeTime[j]);
    }
  }

  outputFile->Write();
  outputFile->Close();
  inputFile->Close();

  return 0;
}