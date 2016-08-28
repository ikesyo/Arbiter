#include "Dependency.h"
#include "Requirement.h"
#include "Resolver.h"
#include "ToString.h"
#include "Value.h"

#include "gtest/gtest.h"

using namespace Arbiter;
using namespace Resolver;

namespace {

ArbiterDependencyList *createEmptyDependencyList (const ArbiterResolver *, const ArbiterProjectIdentifier *, const ArbiterSelectedVersion *, char **)
{
  return new ArbiterDependencyList();
}

ArbiterSelectedVersionList *createEmptyAvailableVersionsList (const ArbiterResolver *, const ArbiterProjectIdentifier *, char **)
{
  return new ArbiterSelectedVersionList();
}

/**
 * A C++ object capable of being an ArbiterUserValue.
 */
class TestValue
{
  public:
    virtual ~TestValue () = default;

    virtual bool operator== (const TestValue &) const = 0;
    virtual bool operator< (const TestValue &) const = 0;
    virtual std::ostream &describe (std::ostream &os) const = 0;

    static ArbiterUserValue convertToUserValue (std::unique_ptr<TestValue> testValue)
    {
      ArbiterUserValue userValue;
      userValue.data = testValue.release();
      userValue.equalTo = &equalTo;
      userValue.lessThan = &lessThan;
      userValue.destructor = &destructor;
      userValue.createDescription = &createDescription;
      return userValue;
    }

  private:
    static bool equalTo (const void *first, const void *second)
    {
      return *static_cast<const TestValue *>(first) == *static_cast<const TestValue *>(second);
    }

    static bool lessThan (const void *first, const void *second)
    {
      return *static_cast<const TestValue *>(first) < *static_cast<const TestValue *>(second);
    }

    static void destructor (void *data)
    {
      delete static_cast<TestValue *>(data);
    }

    static char *createDescription (const void *data)
    {
      return copyCString(toString(*static_cast<const TestValue *>(data))).release();
    }
};

std::ostream &operator<< (std::ostream &os, const TestValue &value)
{
  return value.describe(os);
}

class EmptyTestValue final : public TestValue
{
  public:
    EmptyTestValue () = default;

    bool operator== (const TestValue &other) const override
    {
      return dynamic_cast<const EmptyTestValue *>(&other);
    }

    bool operator< (const TestValue &other) const override
    {
      return !(*this == other);
    }

    std::ostream &describe (std::ostream &os) const override
    {
      return os << "EmptyTestValue";
    }
};

template<typename Owner, typename Value, typename... Args>
SharedUserValue<Owner> makeSharedUserValue (Args &&...args)
{
  return SharedUserValue<Owner>(TestValue::convertToUserValue(std::make_unique<Value>(std::forward<Args>(args)...)));
}

} // namespace

TEST(ResolverTest, ResolvesEmptyDependencies) {
  ArbiterResolverBehaviors behaviors;
  behaviors.createDependencyList = &createEmptyDependencyList;
  behaviors.createAvailableVersionsList = &createEmptyAvailableVersionsList;

  ArbiterResolver resolver(behaviors, ArbiterDependencyList(), makeSharedUserValue<ArbiterResolver, EmptyTestValue>());

  ArbiterResolvedDependencyList resolved = resolver.resolve();
  EXPECT_TRUE(resolved._dependencies.empty());
}
