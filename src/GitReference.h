#ifndef GIT_REFERENCE_HPP
#define GIT_REFERENCE_HPP
#include <functional>
#include <git2.h>
#include <git2/types.h>
#include <memory>
#include <optional>
#include <string>

class GitRepository;

class GitReference {
public:
  GitReference() : ref(nullptr, git_reference_free) {}
  std::string oid() const &;
  std::string shorthand() const &;
  std::optional<GitReference> upstream() const &;

private:
  friend class GitRepository;
  std::unique_ptr<git_reference, decltype(&git_reference_free)> ref;
};
#endif
