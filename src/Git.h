#ifndef GIT_HPP
#define GIT_HPP
#import <git2.h>

class Git {
public:
  Git() { git_libgit2_init(); }
  ~Git() { git_libgit2_shutdown(); }
};
#endif
