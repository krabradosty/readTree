#include "TStyle.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TFile.h"
#include "TProfile.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TROOT.h"
#include <cstdio>
#include <fstream>
#include "TSystem.h"
#include "TF1.h"
#include "PhenixConstants.h"

TStyle* makeplotstyle()
{
	TStyle *style = new TStyle("style", "Style for Summary Plots");
	//style->SetCanvasBorderMode(0);//removes the yellow frame around the canvas
	style->SetFrameBorderMode(0);//removes red highlighting
	style->SetPadTickX(1);//ticks on both pad sides 
	style->SetPadTickY(1);
	style->SetNdivisions(508, "X");
	style->SetNdivisions(508, "Y");

	return style;
}

void drawV39_200(int imeth=1)
{
	const int NG = 3;
	const int NPT200 = 12;
	const int NPADS = 5;
	TFile *file[NG];
	TProfile *prof[NG][NHARM][NCENT5];
	TProfile *pPt[NG][NHARM][NCENT5];
	TProfile *combVProf;//centrality-combined flow profile
    TProfile *combPtProf;
	char ch[200];
	TProfile *statprof;
	
	int ener;
	ofstream outStream[NG];
	for (int ig=0; ig<NG; ig++) {
		ener = (ig==0) ? 39 : 62;
		if (ig==2) ener = 200;
		
		sprintf(ch, "./v2v3_%i_%s_cent0_50.table", ener, GetMethodName(imeth).c_str());
		outStream[ig].open(ch);
	}
		
	//open the files
	sprintf(ch, "./%s/chHSum.root", "./39");
	file[0] = new TFile(ch);
	sprintf(ch, "./%s/chHSum.root", "./62");
	file[1] = new TFile(ch);
	sprintf(ch, "./%s/merged200_201.root", "./200");
	file[2] = new TFile(ch);	
		
	TGraphErrors* gr[NG][NHARM][NPADS];
	Float_t x[NG][NHARM][NPADS][NPT200];
	Float_t y[NG][NHARM][NPADS][NPT200];
	Float_t ey[NG][NHARM][NPADS][NPT200];
	Float_t ex[NPT200];

	//ratio graphs 39/200 and 62/200
	TGraphErrors* rgr[NG-1][NHARM][NPADS];
	Float_t rx[NG-1][NHARM][NPADS][NPTM];
	Float_t ry[NG-1][NHARM][NPADS][NPTM];
	Float_t rey[NG-1][NHARM][NPADS][NPTM];
	
	for (int ipt = 0; ipt<NPT200; ipt++) {
		ex[ipt] = 0;
	}

	//parse profiles
	for (int ig = 0; ig<3; ig++) {
		outStream[ig] << "pt   vn   vn_stat_err"<<endl;
		for (int iharm = 0; iharm<NHARM; iharm++) {
			outStream[ig] << "v" << iharm+2 << endl;
			for (int ic = 0; ic<NPADS*2; ic++) {
				sprintf(ch, "pFlow%s_h%i_%s_%s_xy_c%i_s0", GetMethodName(imeth).c_str(), iharm+2,
					GetRingName(2).c_str(),	GetArmrName(2).c_str(), ic);
				prof[ig][iharm][ic] = (TProfile*)(file[ig]->Get(ch));

				sprintf(ch, "pPt_%i_c%i", iharm+2, ic);
				pPt[ig][iharm][ic] = (TProfile*)(file[ig]->Get(ch));
                
                if (ic%2==1) {
                    prof[ig][iharm][ic]->Add((TH1*)prof[ig][iharm][ic-1]);
                    combVProf = prof[ig][iharm][ic];
                    pPt[ig][iharm][ic]->Add((TH1*)pPt[ig][iharm][ic-1]);
                    combPtProf = pPt[ig][iharm][ic];
                    outStream[ig] << "cent" << ic/2*10 << "-" << ic/2*10+10 << endl;
					for (int ipt = 0; ipt<NPT200; ipt++) {
						if (ig<2 && ipt>=NPT[iharm]) continue;
						y[ig][iharm][ic/2][ipt] = combVProf->GetBinContent(ipt+1);
						ey[ig][iharm][ic/2][ipt] = combVProf->GetBinError(ipt+1);
						x[ig][iharm][ic/2][ipt] = combPtProf->GetBinContent(ipt+1);
						outStream[ig] << x[ig][iharm][ic/2][ipt] << " "
			                          << y[ig][iharm][ic/2][ipt] << " "
			                          << ey[ig][iharm][ic/2][ipt] << endl;
					}
				}
			}		
		}
		outStream[ig].close();
	}
	
	//graphs
	for (int ic = 0; ic<NPADS; ic++) {
		for (int ig = 0; ig<NG; ig++) {
			for (int iharm = 0; iharm<NHARM; iharm++) {
				if (ig<2)
					gr[ig][iharm][ic] = new TGraphErrors(NPT[iharm], &x[ig][iharm][ic][0], &y[ig][iharm][ic][0], ex, &ey[ig][iharm][ic][0]);
				else
					gr[ig][iharm][ic] = new TGraphErrors(NPT200, &x[ig][iharm][ic][0], &y[ig][iharm][ic][0], ex, &ey[ig][iharm][ic][0]);
			}
		}
	}
	
	//fit
	TF1* fit[NHARM][NPADS];
	for (int ic = 0; ic<NPADS; ic++) {
		for (int iharm = 0; iharm<NHARM; iharm++) {
			sprintf(ch, "fit_h%i_c%i", iharm+2, ic);
			fit[iharm][ic] = new TF1(ch, "pol5", 0., 4.);
			fit[iharm][ic]->SetParameters(0,0.05,-0.01,0,0,0);
			fit[iharm][ic]->SetLineColor(kGreen+1);
			fit[iharm][ic]->SetLineWidth(1);
			gr[2][iharm][ic]->Fit(fit[iharm][ic], "R");
		}
	}

	//ratio
	for (int ig=0; ig<NG-1; ig++) {
		for (int iharm = 0; iharm<NHARM; iharm++) {
			for (int ic = 0; ic<NPADS; ic++) {
				for (int ipt = 0; ipt<NPT[iharm]; ipt++) {
					rx[ig][iharm][ic][ipt] = x[ig][iharm][ic][ipt];
					ry[ig][iharm][ic][ipt] = y[ig][iharm][ic][ipt]/fit[iharm][ic]->Eval(x[ig][iharm][ic][ipt]);
					//Float_t rey1 = ey[ig][iharm][ic][ipt]/y[ig][iharm][ic][ipt];
					//Float_t rey0 = ey[2][iharm][ic][ipt]/y[2][iharm][ic][ipt];
					//rey[ig][iharm][ic][ipt] = ry[ig][iharm][ic][ipt] * TMath::Sqrt(rey0*rey0+rey1*rey1);
					rey[ig][iharm][ic][ipt] = ey[ig][iharm][ic][ipt]/fit[iharm][ic]->Eval(x[ig][iharm][ic][ipt]);
				}
			}
		}
	}

	for (int ic = 0; ic<NPADS; ic++) {
		for (int ig = 0; ig<NG-1; ig++) {
			for (int iharm = 0; iharm<NHARM; iharm++) {
				rgr[ig][iharm][ic] = new TGraphErrors(NPT[iharm], &rx[ig][iharm][ic][0], &ry[ig][iharm][ic][0], ex, &rey[ig][iharm][ic][0]);
			}
		}
	}

	//20(24) - circle;  21(25) - square;  31 - *
	//22(26) - up-tri;  23(32) - dw-tri;   5 - x
	//29(30) - star;   33(27) - diamond;  34(28) - cross;
	int markerStyle[NG][NHARM] = {24, 24,
								  23, 23,
								  25, 25};

	int rmarkerStyle[NG-1][NHARM] = {24, 24,
                                     23, 23,};

	Float_t markerSize[NG] = {1., 1., 0.8};
	
	Color_t markerColor[NG] = {kRed+1, kAzure+2, kGreen+1};	
	
	TStyle *tsty = makeplotstyle();
	tsty->cd();
	gROOT->ForceStyle();
	
	TPad* pad[NPADS];
	TPad* rpad[NPADS];
	char padname[10];

	Float_t padXL[NPADS] = {0, 0.26, 0.445, 0.63, 0.815};
	Float_t padXR[NPADS] = {0.26, 0.445, 0.63, 0.815, 1.};
	Float_t padYD = 0.57;

	//margins
	Float_t padLM[NPADS] = {0.29, 0, 0, 0, 0};
    Float_t padRM[NPADS] = {0, 0, 0, 0, 0.01};
	Float_t rpadBM = 0.246;		
	
	TMultiGraph *mg;
	TMultiGraph *rmg;
	TCanvas *c1;
	TLatex *tex[NPADS];
    TLine *line;
	
	float size;
	int energy;
	unsigned char abcChar[5] = {'a','b','c','d','e'};
	
	for (int iharm=0; iharm<NHARM; iharm++){
		c1 = new TCanvas("c1", "title", 651, 320);
		c1->SetFillColor(0);

		for (int ic = 0; ic < NPADS; ic++){
			c1->cd();
			sprintf(ch, "pad%i",ic);
			pad[ic] = new TPad(ch, ch, padXL[ic], padYD, padXR[ic], 1.0);
			pad[ic]->SetMargin(padLM[ic], padRM[ic], 0, 0.01);
			pad[ic]->SetFillColor(0);
			pad[ic]->Draw();
			pad[ic]->Clear();
			pad[ic]->cd();

			mg = new TMultiGraph();
			
			for (int ig=NG-1; ig>=0; ig--) {
				gr[ig][iharm][ic]->SetMarkerSize(markerSize[ig]);
				gr[ig][iharm][ic]->SetMarkerColor(markerColor[ig]);
				gr[ig][iharm][ic]->SetMarkerStyle(markerStyle[ig][iharm]);
				gr[ig][iharm][ic]->SetLineWidth(1);
				gr[ig][iharm][ic]->SetLineColor(markerColor[ig]);
				gr[ig][iharm][ic]->SetDrawOption("P");
				mg->Add(gr[ig][iharm][ic]);
			}
			mg->Draw("AP");
			mg->SetTitle("");

			if (ic == 0) {
				mg->GetYaxis()->SetLabelFont(43);
                mg->GetYaxis()->SetLabelSize(14);
				mg->GetYaxis()->SetLabelOffset(0.01);
				mg->GetYaxis()->SetTitleFont(43);
				mg->GetYaxis()->SetTitleSize(17);
				mg->GetYaxis()->SetTitleOffset(2);
				mg->GetYaxis()->CenterTitle(true);
				sprintf(ch, "v_{%i}", iharm+2);
				mg->GetYaxis()->SetTitle(ch);
			}
			else {
				mg->GetYaxis()->SetTitle("");
				mg->GetYaxis()->SetLabelSize(0);
			}

			mg->GetYaxis()->SetNdivisions(505);
			
			mg->SetMinimum(0.0001);
			float ymax = (iharm==0) ? 0.299 : 0.24;
			mg->SetMaximum(ymax);
			mg->GetXaxis()->SetLimits(-0.12, 3.99);

			if (ic==0) {
				TLegend *legend = new TLegend(0.32,0.42,0.85,0.77);
				legend->SetBorderSize(0);
				legend->SetTextFont(43);
				legend->SetTextSize(13);
				legend->SetFillColor(0);
				legend->AddEntry(gr[0][iharm][ic], "39 GeV", "p"); // l -line, f - box, p - marker
				legend->AddEntry(gr[1][iharm][ic], "62 GeV", "p"); // l -line, f - box, p - marker
				legend->AddEntry(gr[2][iharm][ic], "200 GeV", "p"); // l -line, f - box, p - marker
				/*legend->AddEntry(rgr[0][iharm][ic], "39/200", "p"); // l -line, f - box, p - marker
				legend->AddEntry(rgr[1][iharm][ic], "62/200", "p"); // l -line, f - box, p - marker*/
				legend->Draw("same");
			}
			
			sprintf(ch, "(%c1) %i-%i%%", abcChar[ic], ic*10, ic*10+10);
			tex[ic] = new TLatex(0.3, ymax*0.86, ch);
			tex[ic]->SetTextFont(43);
			tex[ic]->SetTextSize(13);
			tex[ic]->Draw("same");

            line = new TLine(-0.12,0,3.99,0);
            line->SetLineStyle(2);
            line->Draw();

            //ratio
            c1->cd();
			sprintf(ch, "rpad%i",ic);
			rpad[ic] = new TPad(ch, ch, padXL[ic], 0.0, padXR[ic], padYD);
			rpad[ic]->SetMargin(padLM[ic], padRM[ic], rpadBM, 0);
			rpad[ic]->SetFillColor(0);
			rpad[ic]->Draw();
			rpad[ic]->Clear();
			rpad[ic]->cd();

			rmg = new TMultiGraph();
			
			for (int ig=NG-2; ig>=0; ig--) {
				rgr[ig][iharm][ic]->SetMarkerSize(markerSize[ig]);
				rgr[ig][iharm][ic]->SetMarkerColor(markerColor[ig]);
				rgr[ig][iharm][ic]->SetMarkerStyle(rmarkerStyle[ig][iharm]);
				rgr[ig][iharm][ic]->SetLineWidth(1);
				rgr[ig][iharm][ic]->SetDrawOption("P");
				rmg->Add(rgr[ig][iharm][ic]);
			}
			rmg->Draw("AP");
			rmg->SetTitle("");

			if (ic == 0)	{
				rmg->GetYaxis()->SetLabelFont(43);
                rmg->GetYaxis()->SetLabelSize(14);
				rmg->GetYaxis()->SetLabelOffset(0.01);
				rmg->GetYaxis()->SetTitleFont(43);
				rmg->GetYaxis()->SetTitleSize(16);
				rmg->GetYaxis()->SetTitleOffset(2);
				rmg->GetYaxis()->CenterTitle(true);
				sprintf(ch, "Ratio to 200 GeV", iharm+2);
				rmg->GetYaxis()->SetTitle(ch);
			}
			else {
				rmg->GetYaxis()->SetTitle("");
				rmg->GetYaxis()->SetLabelSize(0);
			}
			
			rmg->GetXaxis()->SetLabelFont(43);
			rmg->GetXaxis()->SetLabelSize(15);

			if (ic==2) {
				rmg->GetXaxis()->SetTitleFont(43);
				rmg->GetXaxis()->SetTitleSize(17);
				rmg->GetXaxis()->SetTitleOffset(1.7);
				rmg->GetXaxis()->CenterTitle(true);
				rmg->GetXaxis()->SetTitle("p_{T} [GeV/c]");
			}
			rmg->GetYaxis()->SetNdivisions(506);
			
			rmg->SetMaximum(1.25);
			rmg->SetMinimum(0.75);
			rmg->GetXaxis()->SetLimits(-0.12, 3.99);

			sprintf(ch, "(%c2)", abcChar[ic]);
            tex[ic] = new TLatex(0.3, 1.18, ch);
            tex[ic]->SetTextFont(43);
            tex[ic]->SetTextSize(13);
            tex[ic]->Draw("same");
            
            line = new TLine(-0.12,1.,3.99,1.);
            line->SetLineStyle(2);
            line->Draw();
		}
		sprintf(ch, "./pics/39_200_%s_v%i.png", GetMethodName(imeth).c_str(), iharm+2);
		c1->Print(ch);		
	}
	
	delete c1;
}
