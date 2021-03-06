{
  g=new TFile("./root/TDAna_TTH_120.root");
  g->cd();
  
  TH1D * H1 =  dynamic_cast<TH1D*>(g->Get("Tnotpt"));
  TH1D * H2 =  dynamic_cast<TH1D*>(g->Get("Tpt"));
  TH1D * H3 =  dynamic_cast<TH1D*>(g->Get("THnotproj"));
  TH1D * H4 =  dynamic_cast<TH1D*>(g->Get("THproj"));
  TH1D * H5 =  dynamic_cast<TH1D*>(g->Get("Tnoteta"));
  TH1D * H6 =  dynamic_cast<TH1D*>(g->Get("Teta"));
  TH1D * H7 =  dynamic_cast<TH1D*>(g->Get("THnotdeta"));
  TH1D * H8 =  dynamic_cast<TH1D*>(g->Get("THdeta"));
  TH1D * H9 =  dynamic_cast<TH1D*>(g->Get("THnotdphi"));
  TH1D * H10=  dynamic_cast<TH1D*>(g->Get("THdphi"));
  TH1D * H11=  dynamic_cast<TH1D*>(g->Get("MTnotbest"));
  TH1D * H12=  dynamic_cast<TH1D*>(g->Get("MTbest"));
  H1->Sumw2();
  H2->Sumw2();
  H3->Sumw2();
  H4->Sumw2();
  H5->Sumw2();
  H6->Sumw2();
  H7->Sumw2();
  H8->Sumw2();
  H9->Sumw2();
  H10->Sumw2();
  H11->Sumw2();
  H12->Sumw2();
  double prob = H1->KolmogorovTest(H2);
  cout << "KS test on Top Pt: P=" << prob << endl;
  prob = H3->KolmogorovTest(H4);
  cout <<  "KS test on TH Proj: P=" << prob << endl;
  prob = H5->KolmogorovTest(H6);
  cout <<  "KS test on Top eta: P=" << prob << endl;
  prob = H7->KolmogorovTest(H8);
  cout <<  "KS test on TH delta eta: P=" << prob << endl;
  prob = H9->KolmogorovTest(H10);
  cout <<  "KS test on TH delta phi: P=" << prob << endl;
  prob = H11->KolmogorovTest(H12);
  cout <<  "KS test on T mass: P=" << prob << endl;

  TCanvas * b = new TCanvas ( "b", "Top reconstruction kinematics", 500, 500 );
  b->Divide(2,3);
  
  b->cd(1);
  H2->DrawNormalized("HISTO");
  H1->SetLineColor(kRed);
  H1->SetMarkerColor(kRed);
  H1->SetMarkerSize(0.3);
  H1->DrawNormalized("PESAME");
  b->cd(2);
  H4->DrawNormalized("HISTO");
  H3->SetLineColor(kRed);
  H3->SetMarkerColor(kRed);
  H3->SetMarkerSize(0.3);
  H3->DrawNormalized("PESAME");
  b->cd(3);
  H6->DrawNormalized("HISTO");
  H5->SetLineColor(kRed);
  H5->SetMarkerColor(kRed);
  H5->SetMarkerSize(0.3);
  H5->DrawNormalized("PESAME");
  b->cd(4);
  H8->DrawNormalized("HISTO");
  H7->SetLineColor(kRed);
  H7->SetMarkerColor(kRed);
  H7->SetMarkerSize(0.3);
  H7->DrawNormalized("PESAME");
  b->cd(5);
  H10->DrawNormalized("HISTO");
  H9->SetLineColor(kRed);
  H9->SetMarkerColor(kRed);
  H9->SetMarkerSize(0.3);
  H9->DrawNormalized("PESAME");
  b->cd(6);
  H12->DrawNormalized("HISTO");
  H11->SetLineColor(kRed);
  H11->SetMarkerColor(kRed);
  H11->SetMarkerSize(0.3);
  H11->DrawNormalized("PESAME");
  b->Print("./ps/Tcombs.ps");
}
