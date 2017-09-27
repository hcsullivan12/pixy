#include <iostream>
#include <string>
#include "TFile.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TGaxis.h"
#include "TStyle.h"
#include "TF1.h"
#include "TLegend.h"

void superImpose2Histograms() {
	TFile *f = new TFile("../data/Results.root", "READ");
	std::string h1Name;
	std::string h2Name;
	std::string title;
	std::cout << "Please enter the name of the first histogram: \n";
	std::cin >> h1Name;
	std::cout << "Please enter the name of the second histogram: \n";
	std::cin >> h2Name;
	std::cout << "Enter the title name:\n";
	std::cin.ignore();
	std::getline(std::cin, title);
	std::cout << title << std::endl;
	TH1S *h1 = nullptr;
	TH1S *h2 = nullptr;
	f->GetObject(h1Name.c_str(), h1);
	f->GetObject(h2Name.c_str(), h2);
	if(h1 == NULL) {
		std::cout << "Error. Couldn't find histogram: " << h1Name << std::endl;
		exit(1);
	}
	if(h2 == NULL) {
		std::cout << "Error. Couldn't find histogram: " << h2Name << std::endl;
		exit(1);
	}
	
	TCanvas *c1 = new TCanvas("c1", "Two histograms", 900, 700);
	h1->SetFillColor(kBlack);
	h1->Draw();
	
	h1->SetTitle(title.c_str());
	c1->Update();
	float rightmax = 1.1*h2->GetMaximum();
	float scale = gPad->GetUymax()/rightmax;
	h2->Scale(scale);
	h2->SetFillColor(kRed);
	h2->Draw("histsame");

	TGaxis *axis = new TGaxis(gPad->GetUxmax(), gPad->GetUymin(), gPad->GetUxmax(), gPad->GetUymax(),0,rightmax, 510, "+L");
	axis->SetLabelColor(kRed);
	axis->Draw();

	TLegend *leg = new TLegend(0.1, 0.4, 0.4, 0.1);
	leg->AddEntry(h1, h1Name.c_str(), "f");
	leg->AddEntry(h2, h2Name.c_str(), "f");
	leg->Draw();
	
	gStyle->SetOptStat(0);
	gStyle->SetOptStat(0);
 
}


