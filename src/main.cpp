#include "GitRepository.h"
#include "config.h"
#include "rang.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cxxopts.hpp>
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
}

std::filesystem::path currPath() {
    std::string pwd;
    pwd = std::getenv("PWD");
    if (!pwd.empty()) {
        const std::string powershell_prefix = "Microsoft.PowerShell.Core\\FileSystem::";
        const auto pos = pwd.find(powershell_prefix);
        if (pos != std::string::npos) {
          pwd.erase(pos, powershell_prefix.length());
        }
        return std::filesystem::path(pwd);
    }
    return std::filesystem::current_path();
}

int main(int argc, char *argv[]) {
  // clang-format off
  cxxopts::Options options("git_status_prompt", "A fast git status prompt");
  options.add_options()
    ("c,color", "Force color on",cxxopts::value<bool>()->default_value("false")
                                                       ->implicit_value("true"))
    ("s,status", "Shell status",cxxopts::value<int>()->default_value("0"))
    ("h,help", "Print usage")
    ;
  // clang-format on

  auto result = options.parse(argc, argv);

  if (result.count("help")) {
    std::cout << options.help() << std::endl;
    exit(0);
  }

  if (result["color"].as<bool>())
    rang::setControlMode(rang::control::Force);

  const auto home = homePath();
  const auto curr = currPath();

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
  auto repo = GitRepository(curr.string());
  auto repoStatus = repo.status();
  auto repoHead = repo.head();

  if (repoHead.has_value())
    std::cout << STATUS_SEPARATOR << rang::fg::BRANCH_COLOR << BRANCH_SYMBOL
              << repoHead->shorthand() << rang::fg::reset;

  if (repoStatus.untracked() > 0)
    std::cout << STATUS_SEPARATOR << rang::fg::UNTRACKED_COLOR
              << UNTRACKED_SYMBOL << rang::fg::reset;

  if (repoStatus.staged() > 0)
    std::cout << STATUS_SEPARATOR << rang::fg::STAGED_COLOR << STAGED_SYMBOL
              << rang::fg::reset;

  if (repoStatus.changed() > 0)
    std::cout << STATUS_SEPARATOR << rang::fg::CHANGED_COLOR << CHANGED_SYMBOL
              << rang::fg::reset;

  if (repoStatus.renamed() > 0)
    std::cout << STATUS_SEPARATOR << rang::fg::RENAMED_COLOR << RENAMED_SYMBOL
              << rang::fg::reset;

  if (repoStatus.deleted() > 0)
    std::cout << STATUS_SEPARATOR << rang::fg::DELETED_COLOR << DELETED_SYMBOL
              << rang::fg::reset;

  auto [ahead, behind] = repo.aheadBehind();
  if (ahead > 0)
    std::cout << STATUS_SEPARATOR << rang::fg::AHEAD_COLOR << AHEAD_SYMBOL
              << rang::fg::reset;

  if (behind > 0)
    std::cout << STATUS_SEPARATOR << rang::fg::BEHIND_COLOR << BEHIND_SYMBOL
              << rang::fg::reset;

  if (repoStatus.conflicts() > 0)
    std::cout << STATUS_SEPARATOR << rang::fg::CONFLICTS_COLOR
              << CONFLICTS_SYMBOL << rang::fg::reset;

  if (repoHead.has_value())
    std::cout << STATUS_SEPARATOR << repoHead->oid();

  if (result["status"].as<int>() != 0)
    std::cout << STATUS_SEPARATOR << rang::fg::BAD_EXIT_COLOR << BAD_EXIT_SYMBOL
              << rang::fg::reset;

  std::cout << "\n→ " << std::flush;
  return 0;
}
