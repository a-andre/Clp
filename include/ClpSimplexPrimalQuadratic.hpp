// Copyright (C) 2003, International Business Machines
// Corporation and others.  All Rights Reserved.

/* 
   Authors
   
   John Forrest

 */
#ifndef ClpSimplexPrimalQuadratic_H
#define ClpSimplexPrimalQuadratic_H

class ClpQuadraticInfo;
class ClpQuadraticObjective;

#include "ClpSimplexPrimal.hpp"

/** This solves Quadratic LPs using the primal simplex method

    It inherits from ClpSimplexPrimal.  It has no data of its own and 
    is never created - only cast from a ClpSimplexPrimal object at algorithm time. 
    If needed create new class and pass around

*/

class ClpSimplexPrimalQuadratic : public ClpSimplexPrimal {

public:

  /**@name Description of algorithm */
  //@{
  /** Primal algorithms for quadratic
      At present we have two algorithms:

      a) Dantzig's algorithm 
      b) Using a semi-trust region approach as for pooling problem
         This is in because I have it lying around

  */
  /// A sequential LP method
  int primalSLP(int numberPasses, double deltaTolerance);
  /** Dantzig's method (actually a mixture with Jensen and King)
      phase - 0 normal, 1 getting complementary solution,
      2 getting basic solution. 
      Returns 0 if okay, 1 if LP infeasible.
  */
  int primalQuadratic(int phase=2);
  /** This is done after first pass
      phase - 0 normal, 1 getting complementary solution,
      2 getting basic solution. */
  int primalQuadratic2 (ClpQuadraticInfo * info,
			int phase=2);
  /** This creates the large version of QP and
      fills in quadratic information.
      Returns NULL if no quadratic information
  */
  ClpSimplexPrimalQuadratic * makeQuadratic(ClpQuadraticInfo & info);

  /// This moves solution back
  int endQuadratic(ClpSimplexPrimalQuadratic * quadraticModel,
		   ClpQuadraticInfo & info);
  /// Checks complementarity and computes infeasibilities
  int checkComplementarity (ClpQuadraticInfo * info,
			    CoinIndexedVector * array1, CoinIndexedVector * array2);
  /// Fills in reduced costs
  void createDjs (ClpQuadraticInfo * info,
		  CoinIndexedVector * array1, CoinIndexedVector * array2);
  /** Main part.
      phase - 0 normal, 1 getting complementary solution,
      2 getting basic solution. */
  int whileIterating (ClpQuadraticInfo * info);
  /** 
      Row array has pivot column
      This chooses pivot row.
      Rhs array is used for distance to next bound (for speed)
      For speed, we may need to go to a bucket approach when many
      variables go through bounds
      On exit rhsArray will have changes in costs of basic variables
      Initially no go thru
      Returns 0 - can do normal iteration
      1 - losing complementarity
      cleanupIteration - 0 no, 1 yes, 2 restoring one of x/s in basis
  */
  int primalRow(CoinIndexedVector * rowArray,
		CoinIndexedVector * rhsArray,
		CoinIndexedVector * spareArray,
		CoinIndexedVector * spareArray2,
		ClpQuadraticInfo * info,
		int cleanupIteration);
  /**  Refactorizes if necessary 
       Checks if finished.  Updates status.
       lastCleaned refers to iteration at which some objective/feasibility
       cleaning too place.

       type - 0 initial so set up save arrays etc
            - 1 normal -if good update save
	    - 2 restoring from saved 
  */
  void statusOfProblemInPrimal(int & lastCleaned, int type,
			       ClpSimplexProgress * progress,
			       ClpQuadraticInfo * info);
  //@}

};

/** Trivial class to keep quadratic iterating info around

*/

class ClpQuadraticInfo  {
  
public:
  
public:

  /**@name Constructors, destructor */
  //@{
  /// Default constructor. 
  ClpQuadraticInfo();
  /** Constructor from original model
  */
  ClpQuadraticInfo(const ClpSimplex * model);
  /// Destructor
  ~ClpQuadraticInfo();
  // Copy
  ClpQuadraticInfo(const ClpQuadraticInfo&);
  // Assignment
  ClpQuadraticInfo& operator=(const ClpQuadraticInfo&);
  //@}
     

  /**@name Gets and sets */
  //@{
  /// Number of Original columns
  inline int numberXColumns() const
  {return numberXColumns_;};
  /// Number of Quadratic columns
  inline int numberQuadraticColumns() const
  {return numberQuadraticColumns_;};
  /// Number of Original rows
  inline int numberXRows() const
  {return numberXRows_;};
  /// Number of Quadratic rows
  inline int numberQuadraticRows() const
  {return numberQuadraticRows_;};
  /// Sequence number incoming
  inline int sequenceIn() const
  {return currentSequenceIn_;};
  inline void setSequenceIn(int sequence) 
  {currentSequenceIn_=sequence;};
  /// Sequence number of binding Sj
  inline int crucialSj() const
  {return crucialSj_;};
  inline void setCrucialSj(int sequence) 
  {crucialSj_=sequence;};
  /// Current phase
  inline int currentPhase() const
  {return currentPhase_;};
  inline void setCurrentPhase(int phase) 
  {currentPhase_=phase;};
  /// Current saved solution
  inline const double * currentSolution() const
  {return currentSolution_;};
  void setCurrentSolution(const double * solution);
  /// Returns pointer to original objective
  inline ClpQuadraticObjective * originalObjective() const
  { return originalObjective_;};
  inline void setOriginalObjective( ClpQuadraticObjective * obj)
  { originalObjective_ = obj;};
  /// Quadratic objective
  CoinPackedMatrix * quadraticObjective() const;
  /// Linear objective
  double * linearObjective() const;
  /// Save current In and Sj status
  void saveStatus();
  /// Restore previous
  void restoreStatus();
  ///Dj weights
  inline double * djWeight() const
  { return djWeight_;};
  /// create gradient
  void createGradient(ClpSimplex * model);
  /// Current gradient
  inline double * gradient() const
  { return gradient_;};
  /// Infeas cost
  inline double infeasCost() const
  { return infeasCost_;};
  inline void setInfeasCost(double value)
  { infeasCost_ = value;};
  /// Backward pointer to basis (inverse of pivotVariable_)
  inline int * basicRow() const
  { return basicRow_;};
  /// Set if Sj variable is implied
  inline int * impliedSj() const
  { return impliedSj_;};
  //@}
    
private:
  /**@name Data members */
  //@{
  /// Objective
  ClpQuadraticObjective * originalObjective_;
  /// Backward pointer to basis (inverse of pivotVariable_)
  int * basicRow_;
  /// Set if Sj variable is implied
  int * impliedSj_;
  /// Current sequenceIn
  int currentSequenceIn_;
  /// Crucial Sj
  int crucialSj_;
  /// Valid sequenceIn (for valid factorization)
  int validSequenceIn_;
  /// Valid crucial Sj
  int validCrucialSj_;
  /// Current phase
  int currentPhase_;
  /// Current saved solution
  double * currentSolution_;
  /// Valid phase
  int validPhase_;
  /// Valid saved solution
  double * validSolution_;
  /// Dj weights to stop looping
  double * djWeight_;
  /// Current gradient
  double * gradient_;
  /// Number of original rows
  int numberXRows_;
  /// Number of original columns 
  int numberXColumns_;
  /// Number of quadratic columns 
  int numberQuadraticColumns_;
  /// Number of quadratic rows 
  int numberQuadraticRows_;
  /// Infeasibility cost
  double infeasCost_;
  //@}
};
#endif
