#include "GitRepository.h"
#include "GitReference.h"
#include <cstddef>
#include <git2/graph.h>
#include <git2/refs.h>
#include <optional>
#include <utility>

GitRepositoryStatus GitRepository::status() const & {
  GitRepositoryStatus d;

  git_status_options opts = GIT_STATUS_OPTIONS_INIT;
  opts.flags = GIT_STATUS_OPT_DEFAULTS | GIT_STATUS_OPT_RENAMES_HEAD_TO_INDEX |
               GIT_STATUS_OPT_RENAMES_INDEX_TO_WORKDIR |
               GIT_STATUS_OPT_RENAMES_FROM_REWRITES;

  if (repo)
    git_status_foreach_ext(repo, &opts, git_repository_status_cb, &d);
  return d;
}

std::optional<GitReference> GitRepository::head() const & {
  GitReference gitRef;
  if (repo && git_repository_head(&gitRef.ref, repo) == 0)
    return gitRef;
  return std::nullopt;
}

std::pair<std::size_t, std::size_t> GitRepository::aheadBehind() const & {
  const std::optional<GitReference> local = head();
  if (local) {
    const std::optional<GitReference> upstream = local->upstream();
    if (upstream) {
      auto localOid = git_reference_target(local->ref);
      auto upstreamOid = git_reference_target(upstream->ref);
      size_t ahead = 0, behind = 0;
      git_graph_ahead_behind(&ahead, &behind, repo, localOid, upstreamOid);
      return std::make_pair(ahead, behind);
    }
  }
  return std::make_pair(0, 0);
}

GitRepository::GitRepository() : root(nullptr), repo(nullptr) {
  git_libgit2_init();
}

GitRepository::GitRepository(const std::string &path)
    : root(new git_buf()), repo(nullptr) {
  git_libgit2_init();
  if (git_repository_discover(root, path.c_str(), false, "/") == 0) {
    git_repository_open(&repo, root->ptr);
  }
}

GitRepository::GitRepository(const GitRepository &orig)
    : root(new git_buf()), repo(nullptr) {
  if (git_buf_set(root, orig.root->ptr, orig.root->asize) != 0)
    throw std::runtime_error("Failed allocating git buffer");

  if (git_repository_open(&repo, root->ptr) != 0)
    throw std::runtime_error("Failed opening repository");
}

GitRepository::GitRepository(GitRepository &&orig) noexcept
    : root(std::exchange(orig.root, nullptr)),
      repo(std::exchange(orig.repo, nullptr)) {}

GitRepository &GitRepository::operator=(const GitRepository &rhs) {
  git_buf *newRoot = new git_buf();
  if (git_buf_set(newRoot, rhs.root->ptr, rhs.root->asize) != 0) {
    git_buf_dispose(newRoot);
    delete newRoot;
    throw std::runtime_error("Failed allocating git buffer");
  }

  git_repository *newRepo;
  if (git_repository_open(&newRepo, newRoot->ptr) != 0) {
    git_repository_free(newRepo);
    throw std::runtime_error("Failed opening repository");
  }

  git_repository_free(repo);
  git_buf_dispose(root);
  delete root;

  root = newRoot;
  repo = newRepo;
  return *this;
}

GitRepository &GitRepository::operator=(GitRepository &&orig) noexcept {
  std::swap(root, orig.root);
  std::swap(repo, orig.repo);
  return *this;
}

GitRepository::~GitRepository() {
  git_repository_free(repo);
  git_buf_dispose(root);
  delete root;
  git_libgit2_shutdown();
};
