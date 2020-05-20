#include "GitReference.h"
#include <git2/branch.h>
#include <git2/oid.h>
#include <git2/refs.h>
#include <git2/types.h>
#include <stdexcept>
#include <string>
#include <utility>

std::string GitReference::oid() const & {
  if (ref) {
    std::string oidHex = git_oid_tostr_s(git_reference_target(ref));
    return oidHex.substr(0, 8);
  }
  return "";
}

std::string GitReference::shorthand() const & {
  return ref ? git_reference_shorthand(ref) : "";
}

std::optional<GitReference> GitReference::upstream() const & {
  GitReference gitRef;
  const bool isBranch = git_reference_is_branch(ref);
  if (isBranch && git_branch_upstream(&gitRef.ref, ref) == 0)
    return gitRef;
  return std::nullopt;
}

GitReference::~GitReference() { git_reference_free(ref); };

GitReference::GitReference(const GitReference &orig) {
  if (git_reference_dup(&ref, orig.ref) != 0)
    throw new std::runtime_error("Failed duplicating git reference");
}

GitReference::GitReference(GitReference &&orig) noexcept
    : ref(std::exchange(orig.ref, nullptr)) {}

GitReference &GitReference::operator=(const GitReference &rhs) {
  git_reference *newRef = nullptr;
  if (git_reference_dup(&newRef, rhs.ref) != 0) {
    git_reference_free(newRef);
    throw new std::runtime_error("Failed duplicating git reference");
  }

  git_reference_free(ref);
  ref = newRef;
  return *this;
}

GitReference &GitReference::operator=(GitReference &&rhs) noexcept {
  std::swap(ref, rhs.ref);
  return *this;
}
