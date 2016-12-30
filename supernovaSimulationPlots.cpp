#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include "TApplication.h"

using namespace std;

int main() {
  TApplication *myapp = new TApplication("myapp",0,0);
  Int_t muNpe;
  Float_t ene0;


  TFile* inputFile = new TFile("/home/krab/workdir/simulation/supernova_80PMTs.root");
  TTree* tree =(TTree*) inputFile->Get("dstree");
  //TFile* outputFile = new TFile("/home/krab/workdir/simulation/readTree/out.root", "RECREATE");

  tree->Draw("munpe:ene0","munpmts > 5","CONT4", 100000, 0);

  /*TH2F* hScatter = new TH2F("hScatter", "", 100, 0, 50000, 30, 0, 60);

  tree->SetBranchAddress("munpe", &muNpe);
  tree->SetBranchAddress("ene0", &ene0);
  auto nentries = tree->GetEntries();
  for (long i = 0; i < nentries; i++) {
    tree->GetEntry(i);
    hScatter->Fill(ene0, muNpe);
  }
  */
  myapp->Run();
  //outputFile->Write();
  //outputFile->Close();
  inputFile->Close();

  return 0;
}

