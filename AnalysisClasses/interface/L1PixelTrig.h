#ifndef L1PIXELTRIG_H
#define L1PIXELTRIG_H

#include "DivisiveAlgorithm.h"

template <class T1>
class L1PixelTrig {
 public:
  L1PixelTrig( int PJNUMBER, double DZ = 0, int NUMTKCUT = 0 ) {
    pjnumber_ = PJNUMBER;
    dz_ = DZ;
    numTkCut_ = NUMTKCUT;
    response_ = false;
  }
//  void Fill( vector<const PixelJet> );
  void Fill( const vector<T1> & vec_pj );
  bool Response() const {
    return response_;
  }
  auto_ptr<vector<Vertex<T1> > > VevVertex () {
    return vertex_vec_aptr;
  }
 private:
  int pjnumber_;
  bool response_;
  vector<const T1 *> vec_pj_ptr;
  auto_ptr<vector<Vertex<T1> > > vertex_vec_aptr;
  double dz_;
  int numTkCut_;
};

template <class T1>
//void L1PixelTrig::Fill ( vector<const PixelJet> & vec_pj ) {
void L1PixelTrig<T1>::Fill ( const vector<T1> & vec_pj ) {

  if ( vec_pj.size() != 0 ) {

    // Create a new vector<pointers to obj>, so that the passed vector
    // is not modified in any way.
    // Use only pixeljets with at least numTkCut_ tracks.
    typename vector<T1>::const_iterator vec_pj_it = vec_pj.begin();
    for ( ; vec_pj_it != vec_pj.end(); ++vec_pj_it ) {
      if ( vec_pj_it->tkNum() >= numTkCut_ )
      vec_pj_ptr.push_back( &*(vec_pj_it) );
    }

    //  DivisiveAlgorithm<const PixelJet> div;
    DivisiveAlgorithm<T1> div( dz_ );
    div.Fill( vec_pj_ptr );

    // Get the vector of verteces
    //  auto_ptr<vector<Vertex<PixelJet> > > vertex_vec_aptr( div.VecVertex() );
    vertex_vec_aptr = div.VecVertex();

    if ( vec_pj_ptr.size() > 0 ) {

      // Sort the vector in ascending Pt
      sort( vertex_vec_aptr->begin(), vertex_vec_aptr->end() );

      //     typename vector<Vertex<T1> >::const_iterator it_t = (vertex_vec_aptr->begin());
      //     int c = 0;
      //     for( ; it_t != vertex_vec_aptr->end(); ++it_t, ++c ) {
      //       cout << "trig pt["<<c<<"] = " << it_t->pt() << endl;
      //       cout << "trig eta["<<c<<"] = " << it_t->eta() << endl;
      //       cout << "trig phi["<<c<<"] = " << it_t->phi() << endl;
      //       cout << "trig z["<<c<<"] = " << it_t->z() << endl;
      //       cout << "trig number["<<c<<"] = " << it_t->number() << endl;
      //     }

      if ( (vertex_vec_aptr->back()).number() >= pjnumber_ ) {
        response_ = true;
      }
    }
  }
  else response_ = false;
}
#endif // L1PIXELTRIG_H
