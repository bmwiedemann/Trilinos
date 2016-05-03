#ifndef TEMPUS_INTEGRATORBASIC_HPP
#define TEMPUS_INTEGRATORBASIC_HPP

// Teuchos
#include "Teuchos_VerboseObject.hpp"
#include "Teuchos_Describable.hpp"
#include "Teuchos_ParameterList.hpp"
// Tempus
#include "Thyra_ModelEvaluator.hpp"
#include "Thyra_NonlinearSolverBase.hpp"
// Tempus
#include "Tempus_Integrator.hpp"
#include "Tempus_Stepper.hpp"
#include "Tempus_TimeStepControl.hpp"
#include "Tempus_IntegratorObserver.hpp"

#include <string>

namespace Tempus {


/** \brief Basic time integrator
 */
template<class Scalar>
class IntegratorBasic : virtual public Tempus::Integrator<Scalar>
{
public:

  /** \brief Constructor with ParameterList, model and optional solvers. */
  IntegratorBasic(
    Teuchos::RCP<Teuchos::ParameterList>                     pList,
    const Teuchos::RCP<Thyra::ModelEvaluator<Scalar> >&      model,
    const Teuchos::RCP<Thyra::NonlinearSolverBase<Scalar> >& solver);

  /// Destructor
  virtual ~IntegratorBasic() {}

  /// \name Basic integrator methods
  //@{
    /// Advance the solution to time, and return true if successful.
    virtual bool advanceTime(const Scalar time);
  //@}

  /// \name Overridden from Teuchos::ParameterListAcceptor
  //@{
    virtual void setParameterList(Teuchos::RCP<Teuchos::ParameterList> const& pl);
    virtual Teuchos::RCP<Teuchos::ParameterList> getNonconstParameterList();
    virtual Teuchos::RCP<Teuchos::ParameterList> unsetParameterList();
    virtual Teuchos::RCP<const Teuchos::ParameterList> getValidParameters() const;
  //@}

  /// \name Overridden from Teuchos::Describable
  //@{
    virtual std::string description() const;
    virtual void describe(Teuchos::FancyOStream        & out,
                          const Teuchos::EVerbosityLevel verbLevel) const;
  //@}
  /// \name Accessor methods
  //@{
    /// Get time
    virtual Scalar getTime() const{return workingState->getTime();}
    /// Get index
    virtual Scalar getIndex() const{return workingState->getIndex();}
  //@}

  /// \name Undo type capabilities
  //@{
    /// Only accept step after meeting time step criteria.
    virtual bool acceptTimeStep();
  //@}

protected:

  Teuchos::RCP<Teuchos::ParameterList>      pList;
  Teuchos::RCP<SolutionHistory<Scalar> >    solutionHistory;
  Teuchos::RCP<TimeStepControl<Scalar> >    timeStepControl;
  Teuchos::RCP<IntegratorObserver<Scalar> > integratorObserver;
  Teuchos::RCP<Stepper<Scalar> >            stepper;

  Teuchos::RCP<SolutionState<Scalar> >      currentState; ///< The last accepted state
  Teuchos::RCP<SolutionState<Scalar> >      workingState; ///< The state being worked on

};
} // namespace Tempus


/// Non-member constructor
//template<class Scalar>
//Teuchos::RCP<Tempus::IntegratorBasic<Scalar> > intBasic(
//  Teuchos::RCP<Teuchos::ParameterList>                pList,
//  const Teuchos::RCP<Thyra::ModelEvaluator<Scalar> >& model,
//  const Teuchos::RCP<Thyra::NonlinearSolverBase<Scalar> >& solver)
//{
//  Teuchos::RCP<Tempus::IntegratorBasic<Scalar> > integrator =
//    Teuchos::rcp(new Tempus::IntegratorBasic<Scalar>(pList, model, solver));
//  return(integrator);
//}


#endif // TEMPUS_INTEGRATORBASIC_HPP
