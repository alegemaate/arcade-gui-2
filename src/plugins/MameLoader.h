#pragma once

#include <string>

#include "../mame/Library.h"
#include "LoaderPlugin.h"

/**
 * @brief Lists the rom sets MAME can run
 *
 */
class MameLoader : public LoaderPlugin {
 public:
  /**
   * @brief Construct a new Mame Loader object
   *
   * @param library - Rom sets, refreshed before the plugin is loaded
   */
  explicit MameLoader(mame::Library& library) : library(library) {}

  /**
   * @brief Checks if mame is installed
   *
   * @return true - if mame is installed
   */
  bool validate() override { return library.isAvailable(); }

  /**
   * @brief Add the rom sets found so far
   *
   */
  void load() override;

  /**
   * @brief Revalidate plugin data, does nothing
   *
   */
  void revalidate() override {}

  /**
   * @brief Pick up a finished scan
   *
   * @return true - If the list changed
   */
  bool update() override;

  /**
   * @brief Return name of plugin
   *
   * @return std::string - name of plugin
   */
  std::string getName() const override { return "MameLoader"; }

 private:
  mame::Library& library;

  /// @brief Library generation of the games loaded
  unsigned generation = 0;
};
