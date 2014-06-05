<?php

/**
 * @filesize( )
 */

class DrupalToolKit {

  #################
  ##    FLAGS    ##
  #################

  /**
   * Give the users that has flagged a entity.
   * This is a wrapper of flag_get_entity_flags
   * @see flag_get_entity_flags().
   *
   * @param  $entity_type
   *         The type of entity that will be checked for users. Usually 'node'
   * @param  $entity_id
   *         Entity Id that we're checking.
   * @param  $flag_name (optional)
   *         Flag type.
   */
  public static function getUsersFlaggedContent($entity_type, $entity_id, $flag_name = NULL) {
    $users_flagged = array();
    // Flag module isn't a dependency, we've to be sure that it's enabled.
    if (module_exists('flag')) {
      $users_flagged = flag_get_entity_flags($entity_type, $entity_id, $flag_name);
    }
    return $users_flagged;
  }
}
