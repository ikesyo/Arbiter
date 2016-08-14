#include "Version-inl.h"

#include <ostream>
#include <sstream>

using namespace Arbiter;

Arbiter::Optional<ArbiterSemanticVersion> ArbiterSemanticVersion::fromString (const std::string &versionString)
{
  unsigned major = 0;
  unsigned minor = 0;
  unsigned patch = 0;
  size_t skip = 0;

  int argsRead = sscanf(versionString.c_str(), "%u.%u.%u%zn", &major, &minor, &patch, &skip);
  if (argsRead < 3) {
    return Arbiter::Optional<ArbiterSemanticVersion>();
  }

  Arbiter::Optional<std::string> prereleaseVersion;
  Arbiter::Optional<std::string> buildMetadata;

  if (skip < versionString.length()) {
    std::istringstream stream(versionString.substr(skip));

    char ch;
    stream >> ch;
    if (stream.good()) {
      if (ch == '-') {
        std::string prerelease;
        std::getline(stream, prerelease, '+');
        if (stream.fail()) {
          return Arbiter::Optional<ArbiterSemanticVersion>();
        }

        prereleaseVersion = Arbiter::Optional<std::string>(std::move(prerelease));
        if (!stream.eof()) {
          ch = '+';
        }
      }

      if (ch == '+') {
        std::string metadata;
        stream >> metadata;
        if (stream.fail()) {
          return Arbiter::Optional<ArbiterSemanticVersion>();
        }

        buildMetadata = Arbiter::Optional<std::string>(std::move(metadata));
      } else {
        // Unrecognized part of the string
        return Arbiter::Optional<ArbiterSemanticVersion>();
      }
    }
  }

  return ArbiterSemanticVersion(major, minor, patch, prereleaseVersion, buildMetadata);
}

bool ArbiterSemanticVersion::operator< (const ArbiterSemanticVersion &other) const noexcept
{
  if (_major < other._major) {
    return true;
  } else if (_major > other._major) {
    return false;
  }

  if (_minor < other._minor) {
    return true;
  } else if (_minor > other._minor) {
    return false;
  }

  if (_patch < other._patch) {
    return true;
  } else if (_patch > other._patch) {
    return false;
  }

  if (_prereleaseVersion) {
    if (!other._prereleaseVersion) {
      return true;
    }

    // FIXME: This should compare numbers naturally, not lexically
    return _prereleaseVersion.value() < other._prereleaseVersion.value();
  }

  // Build metadata does not participate in precedence.
  return false;
}

std::ostream &operator<< (std::ostream &os, const ArbiterSemanticVersion &version) {
  os << version._major << '.' << version._minor << '.' << version._patch;

  if (version._prereleaseVersion) {
    os << '-' << version._prereleaseVersion.value();
  }

  if (version._buildMetadata) {
    os << '+' << version._buildMetadata.value();
  }

  return os;
}

unsigned ArbiterGetMajorVersion (const ArbiterSemanticVersion *version) {
  return version->_major;
}

unsigned ArbiterGetMinorVersion (const ArbiterSemanticVersion *version) {
  return version->_minor;
}

unsigned ArbiterGetPatchVersion (const ArbiterSemanticVersion *version) {
  return version->_patch;
}

const char *ArbiterGetPrereleaseVersion (const ArbiterSemanticVersion *version) {
  if (version->_prereleaseVersion) {
    return version->_prereleaseVersion->c_str();
  } else {
    return nullptr;
  }
}

const char *ArbiterGetBuildMetadata (const ArbiterSemanticVersion *version) {
  if (version->_buildMetadata) {
    return version->_buildMetadata->c_str();
  } else {
    return nullptr;
  }
}