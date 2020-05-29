#ifndef GIT_REPOSITORY_HPP
#define GIT_REPOSITORY_HPP
#include "Git.h"
#include "GitReference.h"
#include "GitRepositoryStatus.h"
#include <cstddef>
#include <git2.h>
#include <memory>
#include <optional>
#include <string>
#include <utility>

class GitRepository : Git {
public:
  GitRepository()
      : root(nullptr, freeBufResources), repo(nullptr, git_repository_free) {}
  explicit GitRepository(const std::string &);

  GitRepositoryStatus status() const &;
  std::optional<GitReference> head() const &;
  std::pair<std::size_t, std::size_t> aheadBehind() const &;

private:
  static void freeBufResources(git_buf *);

  std::unique_ptr<git_buf, decltype(&git_buf_free)> root;
  std::unique_ptr<git_repository, decltype(&git_repository_free)> repo;
};
#endif
