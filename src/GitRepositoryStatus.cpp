#include "GitRepositoryStatus.h"
#include <git2.h>

std::size_t GitRepositoryStatus::untracked() { return cnt_untracked; }
std::size_t GitRepositoryStatus::conflicts() { return cnt_conflicts; }
std::size_t GitRepositoryStatus::changed() { return cnt_changed; }
std::size_t GitRepositoryStatus::staged() { return cnt_staged; }
std::size_t GitRepositoryStatus::deleted() { return cnt_deleted; }
std::size_t GitRepositoryStatus::renamed() { return cnt_renamed; }

int git_repository_status_cb(const char *path, unsigned int flags,
                             void *payload) {
  auto status = (GitRepositoryStatus *)payload;

  if (flags & (GIT_STATUS_IGNORED | GIT_STATUS_CURRENT))
    return 0;

  if (flags & (GIT_STATUS_INDEX_NEW | GIT_STATUS_INDEX_MODIFIED))
    status->cnt_staged++;

  if (flags & GIT_STATUS_WT_MODIFIED)
    status->cnt_changed++;

  if (flags & (GIT_STATUS_INDEX_RENAMED | GIT_STATUS_WT_RENAMED |
               GIT_STATUS_INDEX_TYPECHANGE | GIT_STATUS_WT_TYPECHANGE))
    status->cnt_renamed++;

  if (flags & (GIT_STATUS_INDEX_DELETED | GIT_STATUS_WT_DELETED))
    status->cnt_deleted++;

  if (flags & GIT_STATUS_CONFLICTED)
    status->cnt_conflicts++;

  if (flags & GIT_STATUS_WT_NEW)
    status->cnt_untracked++;

  return 0;
}
