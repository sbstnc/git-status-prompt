#include "GitReference.h"
#include <git2.h>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

std::string GitReference::oid() const & {
  if (ref) {
    std::string oidHex = git_oid_tostr_s(git_reference_target(ref.get()));
    return oidHex.substr(0, 7);
  }
  return "";
}

std::string GitReference::shorthand() const & {
  return ref ? git_reference_shorthand(ref.get()) : "";
}

std::optional<GitReference> GitReference::upstream() const & {
  if (!git_reference_is_branch(ref.get()))
    return std::nullopt;

  GitReference gitRef;
  git_reference *upstreamRef = nullptr;

  int err = git_branch_upstream(&upstreamRef, ref.get());
  gitRef.ref =
      std::move(std::unique_ptr<git_reference, decltype(&git_reference_free)>(
          std::move(upstreamRef), git_reference_free));
  if (err)
    return std::nullopt;
  return gitRef;
}
