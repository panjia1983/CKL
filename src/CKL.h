/*************************************************************************\

  Copyright 1999 The University of North Carolina at Chapel Hill.
  All Rights Reserved.

  Permission to use, copy, modify and distribute this software and its
  documentation for educational, research and non-profit purposes, without
  fee, and without a written agreement is hereby granted, provided that the
  above copyright notice and the following three paragraphs appear in all
  copies.

  IN NO EVENT SHALL THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL BE
  LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
  CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE
  USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY
  OF NORTH CAROLINA HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH
  DAMAGES.

  THE UNIVERSITY OF NORTH CAROLINA SPECIFICALLY DISCLAIM ANY
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE
  PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
  NORTH CAROLINA HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT,
  UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

  The authors may be contacted via:

  US Mail:             S. Gottschalk, E. Larsen
                       Department of Computer Science
                       Sitterson Hall, CB #3175
                       University of N. Carolina
                       Chapel Hill, NC 27599-3175

  Phone:               (919)962-1749

  EMail:               geom@cs.unc.edu


\**************************************************************************/

#ifndef CKL_H
#define CKL_H

#include "CKL_Compile.h"
#include "CKL_Internal.h"
#include <vector>

namespace CKL
{

//----------------------------------------------------------------------------
//
//  CKL API Return Values
//
//----------------------------------------------------------------------------

enum CKL_RETURN_VALUES
  {

    CKL_OK = 0,

    // Returned when an API function cannot obtain enough memory to
    // store or process a CKL_Model object.
    CKL_ERR_MODEL_OUT_OF_MEMORY = -1,
  
    // Returned when a CKL query cannot allocate enough storage to
    // compute or hold query information.  In this case, the returned
    // data should not be trusted.
    CKL_ERR_OUT_OF_MEMORY = -2,

    // Returned when an unprocessed model is passed to a function which
    // expects only processed models, such as CKL_Collide() or
    // CKL_Distance().
    CKL_ERR_UNPROCESSED_MODEL = -3,

    // Returned when:
    //       1. AddTri() is called before BeginModel().
    //       2. BeginModel() is called immediately after AddTri().
    // This error code is something like a warning: the invoked
    // operation takes place anyway, and CKL does what makes "most
    // sense", but the returned error code may tip off the client that
    // something out of the ordinary is happenning.
    CKL_ERR_BUILD_OUT_OF_SEQUENCE = -4,

    // Returned when EndModel() is called on a model to which no
    // triangles have been added.  This is similar in spirit to the
    // OUT_OF_SEQUENCE return code, except that the requested operation
    // has FAILED -- the model remains "unprocessed", and the client may
    // NOT use it in queries.
    CKL_ERR_BUILD_EMPTY_MODEL = -5
  };

//----------------------------------------------------------------------------
//
//  CKL_REAL
//
//  The floating point type used throughout the package. The type is defined
//  in CKL_Compile.h, and by default is "double"
//
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//
//  CKL_Model
//
//  A CKL_Model stores geometry to be used in a proximity query.
//  The geometry is loaded with a call to BeginModel(), at least one call to
//  AddTri(), and then a call to EndModel().
//
//  // create a two triangle model, m
//
//  CKL_Model m;
//
//  CKL_REAL p1[3],p2[3],p3[3];  // 3 points will make triangle p
//  CKL_REAL q1[3],q2[3],q3[3];  // another 3 points for triangle q
//
//  // some initialization of these vertices not shown
//
//  m.BeginModel();              // begin the model
//  m.AddTri(p1,p2,p3,0);        // add triangle p
//  m.AddTri(q1,q2,q3,1);        // add triangle q
//  m.EndModel();                // end (build) the model
//
//  The last parameter of AddTri() is the number to be associated with the
//  triangle. These numbers are used to identify the triangles that overlap.
//
//  AddTri() copies into the CKL_Model the data pointed to by the three vertex
//  pointers, so that it is safe to delete vertex data after you have
//  passed it to AddTri().
//
//----------------------------------------------------------------------------
//
//  class CKL_Model  - declaration contained in CKL_Internal.h
//  {
//
//  public:
//    CKL_Model();
//    ~CKL_Model();
//
//    int BeginModel(int num_tris = 8); // preallocate for num_tris triangles;
//                                      // the parameter is optional, since
//                                      // arrays are reallocated as needed
//
//    int AddTri(const CKL_REAL *p1, const CKL_REAL *p2, const CKL_REAL *p3,
//               int id);
//
//    int EndModel();
//    int MemUsage(int msg);  // returns model mem usage in bytes
//                            // prints message to stderr if msg == TRUE
//  };

//----------------------------------------------------------------------------
//
//  CKL_CollideResult
//
//  This saves and reports results from a collision query.
//
//----------------------------------------------------------------------------
//
//  struct CKL_CollideResult - declaration contained in CKL_Internal.h
//  {
//    // statistics
//
//    int NumBVTests();
//    int NumTriTests();
//    CKL_REAL QueryTimeSecs();
//
//    // free the list of contact pairs; ordinarily this list is reused
//    // for each query, and only deleted in the destructor.
//
//    void FreePairsList();
//
//    // query results
//
//    int Colliding();
//    int NumPairs();
//    int Id1(int k);
//    int Id2(int k);
//  };

//----------------------------------------------------------------------------
//
//  CKL_Collide() - detects collision between two CKL_Models
//
//
//  Declare a CKL_CollideResult struct and pass its pointer to collect
//  collision data.
//
//  [R1, T1] is the placement of model 1 in the world &
//  [R2, T2] is the placement of model 2 in the world.
//  The columns of each 3x3 matrix are the basis vectors for the model
//  in world coordinates, and the matrices are in row-major order:
//  R(row r, col c) = R[r][c].
//
//  If CKL_ALL_CONTACTS is the flag value, after calling CKL_Collide(),
//  the CKL_CollideResult object will contain an array with all
//  colliding triangle pairs. Suppose CR is a pointer to the
//  CKL_CollideResult object.  The number of pairs is gotten from
//  CR->NumPairs(), and the ids of the 15'th pair of colliding
//  triangles is gotten from CR->Id1(14) and CR->Id2(14).
//
//  If CKL_FIRST_CONTACT is the flag value, the CKL_CollideResult array
//  will only get the first colliding triangle pair found.  Thus
//  CR->NumPairs() will be at most 1, and if 1, CR->Id1(0) and
//  CR->Id2(0) give the ids of the colliding triangle pair.
//
//----------------------------------------------------------------------------

enum CKL_CONTACT_CONTACT_FLAG
  {
    // find all pairwise intersecting triangles
    CKL_ALL_CONTACTS = 1,

    // report first intersecting tri pair found
    CKL_FIRST_CONTACT = 2
  };

int CKL_Collide(CKL_CollideResult *result,
                CKL_REAL R1[3][3], CKL_REAL T1[3], CKL_Model *o1,
                CKL_REAL R2[3][3], CKL_REAL T2[3], CKL_Model *o2,
                int flag = CKL_ALL_CONTACTS);


#if CKL_BV_TYPE & RSS_TYPE  // this is true by default,
// and explained in CKL_Compile.h

//----------------------------------------------------------------------------
//
//  CKL_DistanceResult
//
//  This saves and reports results from a distance query.
//
//----------------------------------------------------------------------------
//
//  struct CKL_DistanceResult - declaration contained in CKL_Internal.h
//  {
//    // statistics
//
//    int NumBVTests();
//    int NumTriTests();
//    CKL_REAL QueryTimeSecs();
//
//    // The following distance and points established the minimum distance
//    // for the models, within the relative and absolute error bounds
//    // specified.
//
//    CKL_REAL Distance();
//    const CKL_REAL *P1();  // pointers to three CKL_REALs
//    const CKL_REAL *P2();
//  };

//----------------------------------------------------------------------------
//
//  CKL_Distance() - computes the distance between two CKL_Models
//
//
//  Declare a CKL_DistanceResult struct and pass its pointer to collect
//  distance information.
//
//  "rel_err" is the relative error margin from actual distance.
//  "abs_err" is the absolute error margin from actual distance.  The
//  smaller of the two will be satisfied, so set one large to nullify
//  its effect.
//
//  "qsize" is an optional parameter controlling the size of a priority
//  queue used to direct the search for closest points.  A larger queue
//  can help the algorithm discover the minimum with fewer steps, but
//  will increase the cost of each step. It is not beneficial to increase
//  qsize if the application has frame-to-frame coherence, i.e., the
//  pair of models take small steps between each call, since another
//  speedup trick already accelerates this situation with no overhead.
//
//  However, a queue size of 100 to 200 has been seen to save time in a
//  planning application with "non-coherent" placements of models.
//
//----------------------------------------------------------------------------

int CKL_Distance(CKL_DistanceResult *result,
                 CKL_REAL R1[3][3], CKL_REAL T1[3], CKL_Model *o1,
                 CKL_REAL R2[3][3], CKL_REAL T2[3], CKL_Model *o2,
                 CKL_REAL rel_err, CKL_REAL abs_err,
                 int qsize = 2);

//----------------------------------------------------------------------------
//
//  CKL_ToleranceResult
//
//  This saves and reports results from a tolerance query.
//
//----------------------------------------------------------------------------
//
//  struct CKL_ToleranceResult - declaration contained in CKL_Internal.h
//  {
//    // statistics
//
//    int NumBVTests();
//    int NumTriTests();
//    CKL_REAL QueryTimeSecs();
//
//    // If the models are closer than ( <= ) tolerance, these points
//    // and distance were what established this.  Otherwise,
//    // distance and point values are not meaningful.
//
//    CKL_REAL Distance();
//    const CKL_REAL *P1();
//    const CKL_REAL *P2();
//
//    // boolean says whether models are closer than tolerance distance
//
//    int CloserThanTolerance();
//  };

//----------------------------------------------------------------------------
//
// CKL_Tolerance() - checks if distance between CKL_Models is <= tolerance
//
//
// Declare a CKL_ToleranceResult and pass its pointer to collect
// tolerance information.
//
// The algorithm returns whether the true distance is <= or >
// "tolerance".  This routine does not simply compute true distance
// and compare to the tolerance - models can often be shown closer or
// farther than the tolerance more trivially.  In most cases this
// query should run faster than a distance query would on the same
// models and configurations.
//
// "qsize" again controls the size of a priority queue used for
// searching.  Not setting qsize is the current recommendation, since
// increasing it has only slowed down our applications.
//
//----------------------------------------------------------------------------

int CKL_Tolerance(CKL_ToleranceResult *res,
                  CKL_REAL R1[3][3], CKL_REAL T1[3], CKL_Model *o1,
                  CKL_REAL R2[3][3], CKL_REAL T2[3], CKL_Model *o2,
                  CKL_REAL tolerance,
                  int qsize = 2);

#endif


//----------------------------------------------------------------------------
//
// CKL_Continuous Collision Checking
//
//
// Declare a CKL_ContinuousCollisionResult and pass its pointer to collect
// continuous collision information.
//
// The algorithm returns whether the collision happens during the continuous
// motion, in terms of the linear interpolation between the starting and end
// configurations of the objects. 
//
// If a collision happens, this routine returns the time of contact
// and the in-contact (or in small collision) configurations of the objects.
// This routine uses the simplest manner to check continuous collision: sample N
// configurations along the motion and check the collisions in order.
//
// The precision of the time_of_contact is determined by N, the number of samples
// along the motion.
// (R11, T11) is the starting configuration of o1 and (R12, T12) is the end configuration
// of o1;
// (R21, T21) is the starting configuration of o2 and (R22, T22) is the end configuration
// of o2.
//
//----------------------------------------------------------------------------
int CKL_ContinuousCollide(CKL_ContinuousCollideResult *result,
                          CKL_REAL R11[3][3], CKL_REAL T11[3], CKL_REAL R12[3][3], CKL_REAL T12[3], CKL_Model *o1,
                          CKL_REAL R21[3][3], CKL_REAL T21[3], CKL_REAL R22[3][3], CKL_REAL T22[3], CKL_Model *o2,
                          int N = 50);




//----------------------------------------------------------------------------
//
// CKL_PenetrationDepth() - checks the global penetration depth between CKL_Models
//
//
// Declare a CKL_PenetrationDepthResult and pass its pointer to collect
// tolerance information.
//
// The algorithm returns the global penetration depth and the related contact point/normal.
//
//----------------------------------------------------------------------------
struct Transform
{
  CKL_REAL R[3][3];
  CKL_REAL T[3];

  Transform(const CKL_REAL R_[3][3], const CKL_REAL T_[3]);
  Transform();
};

std::vector<Transform> CKL_PenetrationDepthModelLearning(CKL_Model* o1, CKL_Model* o2,
                                                         std::size_t n_samples,
                                                         std::size_t knn_k,
                                                         CKL_REAL margin = 0);


int CKL_PenetrationDepth(CKL_PenetrationDepthResult* res,
                         CKL_REAL R1[3][3], CKL_REAL T1[3], CKL_Model *o1,
                         CKL_REAL R2[3][3], CKL_REAL T2[3], CKL_Model *o2,
                         const std::vector<Transform>& contact_space);
                         



}

#endif






