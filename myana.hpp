#ifndef _MYANA_HPP_
#define _MYANA_HPP_

#include <vector>
#include <TROOT.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TFile.h>

#include "anapa.h"

class MyAna
{
public:
  ~MyAna();
  static MyAna &GetInstance()
  {
    static MyAna instance;
    return instance;
  };
  void Analysis(struct evtdata *evt);
  void TreeDef();
  void TreeFill();
  void AutoSave();
  void MakeFile(char *fname);
  void CloseFile();
  void Init();
  void InitLocal();

protected:
  TFile *file;
  int save_flag; // Is tree saved?

  TTree *tree;
  std::vector<TH1D*> hist1d;
  std::vector<TH2D*> hist2d;
  TH2D *map_a;
  TH2D *map_c;

  // analysis values
  long long int evt_num; // number of events
  std::vector<std::vector<std::vector<int> > > l_edge; // leading edge
  std::vector<std::vector<std::vector<int> > > t_edge; // trailing edge
  ULong64_t Map[N_AC][N_STRP][16];
  int edge_pos[2][N_AC][N_STRP][10]; // buff of edge to save
  int edge_num[2][N_AC][N_STRP]; // number of edge
//  std::vector<int> l_edge[2][256];
//  std::vector<int> t_edge[2][256];
  double p[3]; // fit parameters p0, p1, chi2
  double drift_v; // drift speed [mm/ns]
  double tpc_clk; // [s] ?
  double integ_fadc[N_AC][N_FADC];
  double integ_fadc_full[N_AC];
  double peak[N_AC][N_FADC];
  double base_line[N_AC][N_FADC];
  int sca[N_SCA];

private:
  MyAna();
};

#endif
