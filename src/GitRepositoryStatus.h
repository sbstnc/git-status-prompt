#ifndef GIT_REPOSITORY_STATUS_HPP
#define GIT_REPOSITORY_STATUS_HPP
#include <cstddef>

class GitRepositoryStatus {
public:
  GitRepositoryStatus() = default;
  std::size_t untracked();
  std::size_t conflicts();
  std::size_t changed();
  std::size_t staged();
  std::size_t deleted();
  std::size_t renamed();

private:
  friend int git_repository_status_cb(const char *path, unsigned int flags,
                                      void *payload);
  std::size_t cnt_untracked = 0;
  std::size_t cnt_conflicts = 0;
  std::size_t cnt_changed = 0;
  std::size_t cnt_staged = 0;
  std::size_t cnt_deleted = 0;
  std::size_t cnt_renamed = 0;
};

int git_repository_status_cb(const char *path, unsigned int flags,
                             void *payload);
#endif
