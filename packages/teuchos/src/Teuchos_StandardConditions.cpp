// @HEADER
// ***********************************************************************
// 
//                    Teuchos: Common Tools Package
//                 Copyright (2004) Sandia Corporation
// 
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
// 
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1 of the
// License, or (at your option) any later version.
//  
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//  
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
// Questions? Contact Michael A. Heroux (maherou@sandia.gov) 
// 
// ***********************************************************************
// @HEADER



#include "Teuchos_StandardConditions.hpp"

namespace Teuchos{

ParameterCondition::ParameterCondition(std::string parameterName, Teuchos::RCP<Teuchos::ParameterList> parentList, bool whenParamEqualsValue):
  parameterName_(parameterName),
  parentList_(parentList),
  whenParamEqualsValue_(whenParamEqualsValue)
{
  parameter_ = parentList_->getEntryPtr(parameterName);
  if(getParameter() == NULL){
    throw InvalidConditionException("Oh noes!!!!!!! Looks like the parameter " +
    parameterName + " isn't actually contained in the " + parentList->name() + " parameter list. "
    "You should go back and check your code. Maybe the information below can help you.\n\n"
    "Error: Parameter not contained in specified Parent list:\n"
    "Problem Parameter: " + parameterName_ + "\n"
    "Problem List: " + parentList_->name());
  }
}

Dependency::ParameterParentMap ParameterCondition::getAllParameters() const{
  Dependency::ParameterParentMap toReturn;
  toReturn.insert(std::pair<std::string, RCP<ParameterList> >(parameterName_, parentList_));
  return toReturn;
}

BinaryLogicalCondition::BinaryLogicalCondition(ConditionList& conditions):
  conditions_(conditions)
{
  if(conditions_.size() ==0){
    throw InvalidConditionException("Sorry bud, but you gotta at least give me one condition "
    "when you're constructing a BinaryLogicalCondition. Looks like you didn't. I'm just gonna "
    "chalk it up a silly little mistake though. Take a look over your conditions again and make sure "
    "you don't ever give any of your BinaryLogicConditions and empty condition list.\n\n"
    "Error: Empty condition list given to a BinaryLogicalCondition constructor.");
  }
}


void BinaryLogicalCondition::addCondition(RCP<Condition> toAdd){
  conditions_.append(toAdd);
}

bool BinaryLogicalCondition::isConditionTrue() const{
  ConditionList::const_iterator it = conditions_.begin();
  bool toReturn = (*it)->isConditionTrue();
  ++it;
  for(;it != conditions_.end(); ++it){
    toReturn = applyOperator(toReturn,(*it)->isConditionTrue());
  }
  return toReturn;
}

bool BinaryLogicalCondition::containsAtLeasteOneParameter() const{
  for(ConditionList::const_iterator it=conditions_.begin(); it!=conditions_.end(); ++it){
    if((*it)->containsAtLeasteOneParameter()){
      return true;
    }
  }
  return false;
}

Dependency::ParameterParentMap BinaryLogicalCondition::getAllParameters() const{
  Dependency::ParameterParentMap toReturn;
  Dependency::ParameterParentMap currentMap;
  for(ConditionList::const_iterator it = conditions_.begin(); it != conditions_.end(); ++it){
    currentMap = (*it)->getAllParameters();
    toReturn.insert(currentMap.begin(), currentMap.end());
  }
  return toReturn;
}

OrCondition::OrCondition(ConditionList& conditions):
  BinaryLogicalCondition(conditions){}

bool OrCondition::applyOperator(bool op1, bool op2) const{
  return op1 || op2;
}

AndCondition::AndCondition(ConditionList& conditions):
  BinaryLogicalCondition(conditions){}

bool AndCondition::applyOperator(bool op1, bool op2) const{
  return op1 && op2;
}

EqualsCondition::EqualsCondition(ConditionList& conditions):
  BinaryLogicalCondition(conditions){}

bool EqualsCondition::applyOperator(bool op1, bool op2) const{
  return op1 == op2;
}

NotCondition::NotCondition(RCP<Condition> childCondition):
  childCondition_(childCondition)
{
  if(childCondition_.is_null()){
    throw InvalidConditionException("OOOOOOOOPppppps! Looks like you tried to give me "
    "a null pointer when you were making a not conditon. That's a no no. Go back and "
    "checkout your not conditions and make sure you didn't give any of them a null pointer "
    "as an argument to the constructor.\n\n"
    "Error: Null pointer given to NotCondition constructor.");
  }
}

bool NotCondition::isConditionTrue() const{
  return (!childCondition_->isConditionTrue());
}

bool NotCondition::containsAtLeasteOneParameter() const{
  return childCondition_->containsAtLeasteOneParameter();
}

Dependency::ParameterParentMap NotCondition::getAllParameters() const{
  return condition_->getAllParameters();
}

StringCondition::StringCondition(
  std::string parameterName, RCP<ParameterList> parentList, 
  std::string value, bool whenParamEqualsValue):
  ParameterCondition(parameterName, parentList, whenParamEqualsValue), 
  values_(ValueList(1,value))
{
  if(!getParameter()->isType<std::string>()){
    throw InvalidConditionException("The parameter of a String Condition "
    "must be of type string. \n"
    "Expected type: std::string\n"
    "Actual type: " + getParameter()->getAny().typeName() + "\n");
  }
}

StringCondition::StringCondition(
  std::string parameterName, RCP<ParameterList> parentList, ValueList values,
  bool whenParamEqualsValue):
  ParameterCondition(parameterName, parentList, whenParamEqualsValue), 
  values_(values)
{
  if(!getParameter()->isType<std::string>()){
    throw InvalidConditionException("The parameter of a String Condition "
    "must be of type string. \n"
    "Expected type: std::string\n"
    "Actual type: " + getParameter()->getAny().typeName() + "\n");
  }
}

bool StringCondition::evaluateParameter() const{
  return find(
    values_.begin(), values_.end(), 
    getValue<std::string>(*getParameter())) != values_.end();
}

BoolCondition::BoolCondition(
  std::string parameterName, RCP<ParameterList> parentList, 
  bool whenParamEqualsValue):
  ParameterCondition(parameterName, parentList, whenParamEqualsValue)
{
  if(!getParameter()->isType<bool>()){
    throw InvalidConditionException("The parameter of a Bool Condition "
    "must be of type Bool. \n"
    "Expected type: Bool\n"
    "Actual type: " + getParameter()->getAny().typeName() + "\n");
  }
}

bool BoolCondition::evaluateParameter() const{
  return getValue<bool>(*getParameter());
}

} //namespace Teuchos

