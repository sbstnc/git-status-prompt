#include "GitRepository.h"
#include "GitReference.h"
#include <cstddef>
#include <git2.h>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>

GitRepositoryStatus GitRepository::status() const & {
  GitRepositoryStatus d;

  git_status_options opts = GIT_STATUS_OPTIONS_INIT;
  opts.flags = GIT_STATUS_OPT_DEFAULTS | GIT_STATUS_OPT_RENAMES_HEAD_TO_INDEX |
               GIT_STATUS_OPT_RENAMES_INDEX_TO_WORKDIR |
               GIT_STATUS_OPT_RENAMES_FROM_REWRITES;

  if (repo)
    git_status_foreach_ext(repo.get(), &opts, git_repository_status_cb, &d);
  return d;
}

std::optional<GitReference> GitRepository::head() const & {
  if (!repo)
    return std::nullopt;

  GitReference gitRef;
  git_reference *headRef = nullptr;

  int err = git_repository_head(&headRef, repo.get());
  gitRef.ref =
      std::move(std::unique_ptr<git_reference, decltype(&git_reference_free)>(
          headRef, git_reference_free));
  if (err)
    return std::nullopt;
  return gitRef;
}

std::pair<std::size_t, std::size_t> GitRepository::aheadBehind() const & {
  const std::optional<GitReference> local = head();
  if (local) {
    const std::optional<GitReference> upstream = local->upstream();
    if (upstream) {
      auto localOid = git_reference_target(local->ref.get());
      auto upstreamOid = git_reference_target(upstream->ref.get());
      size_t ahead = 0, behind = 0;
      git_graph_ahead_behind(&ahead, &behind, repo.get(), localOid,
                             upstreamOid);
      return std::make_pair(ahead, behind);
    }
  }
  return std::make_pair(0, 0);
}

GitRepository::GitRepository(const std::string &path)
    : root(new git_buf(), &GitRepository::freeBufResources),
      repo(nullptr, [](git_repository *) {}) {
  int err = 0;

  auto *buf = new git_buf();
  err = git_repository_discover(buf, path.c_str(), false, "/");
  auto rootPtr = std::unique_ptr<git_buf, decltype(&git_buf_free)>(
      std::move(buf), &GitRepository::freeBufResources);
  if (err && err != GIT_ENOTFOUND)
    throw std::runtime_error("Failed discovering git repository");
  root = std::move(rootPtr);

  if (err != GIT_ENOTFOUND) {
    git_repository *repoCptr = nullptr;
    err = git_repository_open(&repoCptr, root->ptr);
    auto repoPtr =
        std::unique_ptr<git_repository, decltype(&git_repository_free)>(
            std::move(repoCptr), git_repository_free);
    if (err)
      throw std::runtime_error("Failed opening repository");
    repo = std::move(repoPtr);
  }
}

void GitRepository::freeBufResources(git_buf *buf) {
  git_buf_dispose(buf);
  delete buf;
}
