#include "setdirs.h"

TH2F* getplot(TString filename);
//TGraph* getpoint(TString filename);

void overlayRoc(){

  gStyle->SetOptStat(kFALSE);
  gStyle->SetFillColor(0);


  TCanvas c;

  TH2F* roc =getplot(plotsDir+"signif_BDT_barrel.root");
  TH2F* roc0=getplot(plotsDir+"signif_BDT_barrel_0.root");
  TH2F* roc1=getplot(plotsDir+"signif_BDT_barrel_1.root");
  TH2F* roc2=getplot(plotsDir+"signif_BDT_barrel_2.root");
  TH2F* roc3=getplot(plotsDir+"signif_BDT_barrel_merged.root");
  roc->GetXaxis()->SetRangeUser(0.,1);
  roc->GetYaxis()->SetRangeUser(0.99,1.);

  roc ->SetMarkerColor(1);
  roc0->SetMarkerColor(2);
  roc1->SetMarkerColor(3);
  roc2->SetMarkerColor(4);  
  roc3->SetMarkerColor(5);  

  roc ->SetMarkerStyle(24);
  roc0->SetMarkerStyle(25);
  roc1->SetMarkerStyle(26);
  roc2->SetMarkerStyle(27);
  roc3->SetMarkerStyle(28);

  roc ->SetMarkerSize(0.4);
  roc0->SetMarkerSize(0.4);
  roc1->SetMarkerSize(0.4);
  roc2->SetMarkerSize(0.4);
  roc3->SetMarkerSize(0.8);

  roc ->Draw();
  roc0->Draw("same");
  roc1->Draw("same");
  roc2->Draw("same");
  roc3->Draw("same");

  /*
  double x, y; 
  x = getpoint("plots/signif_BDT_barrel.root")->GetX()[0];
  y = getpoint("plots/signif_BDT_barrel.root")->GetY()[0];
  printf("cccccc %f   %f\n ",x,y);
  TMarker* tm = new TMarker(x,y, 28);
  tm->SetMarkerSize(1.5);
  tm->SetMarkerColor(2);
  tm->Draw("same");
  */

  TLegend* leg = new TLegend(0.7,0.7,0.9,0.9);
  //leg->SetHeader("The Legend Title");
  leg->AddEntry(roc,"full","p");
  leg->AddEntry(roc0,"0","p");
  leg->AddEntry(roc1,"1","p");
  leg->AddEntry(roc2,"2","p");
  leg->AddEntry(roc2,"combined","p");
  leg->Draw();
  
  c.SaveAs("plots/roc_overlay_barrel.pdf");
}


TH2F* getplot(TString filename){
  TFile::Open(filename);
  TH2F* h = (TH2F*)gROOT->FindObject(TString("roc"));
  return h;
}

//TGraph* getpoint(TString filename){
//  TFile::Open(filename);
//  return (TGraph*)gROOT->FindObject(TString("mark"));
//}
