#ifndef GIT_REPOSITORY_HPP
#define GIT_REPOSITORY_HPP
#include "GitReference.h"
#include "GitRepositoryStatus.h"
#include <cstddef>
#include <functional>
#include <git2.h>
#include <optional>
#include <string>
#include <utility>

class GitRepository {
public:
  GitRepository();
  explicit GitRepository(const std::string &);
  GitRepository(const GitRepository &);
  GitRepository(GitRepository &&) noexcept;
  GitRepository &operator=(const GitRepository &);
  GitRepository &operator=(GitRepository &&) noexcept;
  ~GitRepository();

  GitRepositoryStatus status() const &;
  std::optional<GitReference> head() const &;
  std::pair<std::size_t, std::size_t> aheadBehind() const &;

private:
  git_buf *root;
  git_repository *repo;
};
#endif
