#ifndef GIT_REFERENCE_HPP
#define GIT_REFERENCE_HPP
#include <functional>
#include <git2.h>
#include <optional>
#include <string>

class GitRepository;

class GitReference {
public:
  GitReference() : ref(nullptr) {};
  GitReference(const GitReference &);
  GitReference(GitReference &&) noexcept;
  GitReference &operator=(const GitReference &);
  GitReference &operator=(GitReference &&) noexcept;
  ~GitReference();

  std::string oid() const &;
  std::string shorthand() const &;
  std::optional<GitReference> upstream() const &;

private:
  friend class GitRepository;
  git_reference *ref;
};
#endif
