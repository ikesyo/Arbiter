#pragma once

#ifndef __cplusplus
#error "This file must be compiled as C++."
#endif

#include "Requirement.h"
#include "Value.h"

#include <functional>
#include <memory>
#include <ostream>
#include <vector>

struct ArbiterProjectIdentifier
{
  public:
    Arbiter::SharedUserValue _value;

    explicit ArbiterProjectIdentifier (Arbiter::SharedUserValue value)
      : _value(std::move(value))
    {}

    bool operator== (const ArbiterProjectIdentifier &other) const
    {
      return _value == other._value;
    }
};

std::ostream &operator<< (std::ostream &os, const ArbiterProjectIdentifier &identifier);

struct ArbiterDependency
{
  public:
    ArbiterProjectIdentifier _projectIdentifier;

    ArbiterDependency (ArbiterProjectIdentifier projectIdentifier, const ArbiterRequirement &requirement)
      : _projectIdentifier(std::move(projectIdentifier))
      , _requirement(requirement.clone())
    {}

    ArbiterDependency (const ArbiterDependency &other)
      : ArbiterDependency(other._projectIdentifier, other.requirement())
    {}

    ArbiterDependency &operator= (const ArbiterDependency &other);

    const ArbiterRequirement &requirement() const noexcept
    {
      return *_requirement;
    }

    bool operator== (const ArbiterDependency &other) const
    {
      return _projectIdentifier == other._projectIdentifier && *_requirement == *(other._requirement);
    }

  private:
    std::unique_ptr<ArbiterRequirement> _requirement;
};

std::ostream &operator<< (std::ostream &os, const ArbiterDependency &dependency);

struct ArbiterDependencyList
{
  public:
    std::vector<ArbiterDependency> _dependencies;

    explicit ArbiterDependencyList (std::vector<ArbiterDependency> dependencies)
      : _dependencies(std::move(dependencies))
    {}
};

std::ostream &operator<< (std::ostream &os, const ArbiterDependencyList &dependencyList);

namespace std {

template<>
struct hash<ArbiterProjectIdentifier>
{
  public:
    size_t operator() (const ArbiterProjectIdentifier &) const
    {
      // TODO: Need a real hash!
      return 4;
    }
};

template<>
struct hash<ArbiterDependency>
{
  public:
    size_t operator() (const ArbiterDependency &dependency) const
    {
      return Arbiter::hashOf(dependency._projectIdentifier)
        ^ Arbiter::hashOf(dependency.requirement());
    }
};

}