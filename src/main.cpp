#include "GitRepository.h"
#include "config.h"
#include "rang.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <functional>
#include <git2.h>
#include <iostream>
#include <iterator>
#include <limits.h>
#include <numeric>
#include <ostream>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <unistd.h>

std::filesystem::path homePath() {
  std::string home;
#ifdef IS_UNIX
  home = std::getenv("HOME");
#else
  std::cout << "here" << std::endl;
  if (home.empty()) {
    home = std::getenv("USERPROFILE");
    if (home.empty()) {
      home = std::getenv("HOMEDRIVE") + std::getenv("HOMEPATH");
      if (home.empty()) {
        ret = std::getenv("HOME");
      }
    }
  }
#endif
  if (home.empty()) {
    home = "/";
  }
  return std::filesystem::path(std::move(home));
  ;
}

int main(int argc, const char *argv[]) {
  rang::setControlMode(rang::control::Force);
  const auto home = homePath();
  const auto curr = std::filesystem::current_path();

  const auto path_mismatches =
      std::mismatch(curr.begin(), curr.end(), home.begin(), home.end());
  std::string path_str =
      (home.string() != "/" && path_mismatches.second == home.end()) ? "~/"
                                                                     : "/";

  for (auto x = path_mismatches.first, y = curr.end(); x != y; ++x) {
    if (std::next(x) != curr.end()) {
      path_str += x->string().substr(0, 1);
      path_str += std::filesystem::path::preferred_separator;
    } else {
      path_str += x->string();
    }
  }

  std::cout << rang::fg::cyan << path_str << rang::fg::reset;

  auto repo = GitRepository(".");
  auto repoStatus = repo.status();
  auto repoHead = repo.head();

  if (repoHead.has_value())
    std::cout << STATUS_SEPARATOR << rang::fg::green << BRANCH_SYMBOL
              << repoHead->shorthand() << rang::fg::reset;

  if (repoStatus.untracked() > 0)
    std::cout << STATUS_SEPARATOR << rang::fg::red << UNTRACKED_SYMBOL
              << rang::fg::reset;

  if (repoStatus.staged() > 0)
    std::cout << STATUS_SEPARATOR << rang::fg::cyan << STAGED_SYMBOL
              << rang::fg::reset;

  if (repoStatus.changed() > 0)
    std::cout << STATUS_SEPARATOR << rang::fg::yellow << CHANGED_SYMBOL
              << rang::fg::reset;

  if (repoStatus.renamed() > 0)
    std::cout << STATUS_SEPARATOR << rang::fg::green << RENAMED_SYMBOL
              << rang::fg::reset;

  if (repoStatus.deleted() > 0)
    std::cout << STATUS_SEPARATOR << rang::fg::red << DELETED_SYMBOL
              << rang::fg::reset;

  auto [ahead, behind] = repo.aheadBehind();
  if (ahead > 0)
    std::cout << STATUS_SEPARATOR << rang::fg::cyan << AHEAD_SYMBOL
              << rang::fg::reset;

  if (behind > 0)
    std::cout << STATUS_SEPARATOR << rang::fg::red << BEHIND_SYMBOL
              << rang::fg::reset;

  if (repoStatus.conflicts() > 0)
    std::cout << STATUS_SEPARATOR << rang::fg::yellow << CONFLICTS_SYMBOL
              << rang::fg::reset;

  if (repoHead.has_value())
    std::cout << STATUS_SEPARATOR << repoHead->oid();

  if (argc > 1 && std::string(argv[1]) != "0")
    std::cout << STATUS_SEPARATOR << rang::fg::red << BAD_EXIT_SYMBOL
              << rang::fg::reset;

  std::cout << "\n→ " << std::flush;
  return 0;
}
