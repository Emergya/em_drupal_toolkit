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

  ##################
  ##   TAXONOMY   ##
  ##################

  /**
   * @brief Get an array of terms filtered by the string $term_name, the array
   *        can be keyed by term_id ($key_name: False) or term_name ($key_name: TRUE).
   *        This method is userful when we need a autocomplete of terms
   * @return
   */
  public static function taxonomyGetTermsNamesFiltered($vocab_machine_name, $term_name = '', $parent = 0, $key_name = FALSE) {
    $terms = array();
    // Get the vocabulary ID:
    $vocabulary = taxonomy_vocabulary_machine_name_load($vocab_machine_name);

    if (!empty($vocabulary)) {
      $query = db_select('taxonomy_term_data', 't');
      $query->join('taxonomy_term_hierarchy', 'tth', 't.tid = tth.tid');
      $query->fields('t', array('tid', 'name'));
      $query->condition('tth.parent', $parent);
      if (!empty($term_name)) {
        $query->condition('t.name', db_like($term_name) . '%', 'LIKE');
      }
      $results = $query->range(0, 4)
        ->execute();

      foreach ($results as $key => $term) {
        $key = $key_name ? $term->name : $term->tid;
        $terms[$key] = $term->name;
      }
    }
    return $terms;
  }


  /**
   * @brief Get an array of terms filtered by the string $term_name, the array
   *        can be keyed by term_id ($key_name: False) or term_name ($key_name: TRUE).
   *        This method is userful to find a three level hierarchy
   * @return
   */
  public static function taxonomyGetTermsNamesFilteredThreeLevels($vocab_machine_name, $term_name = '', $parent = 0, $key_name = FALSE) {
    $terms = array();
    // Get the vocabulary ID:
    $vocabulary = taxonomy_vocabulary_machine_name_load($vocab_machine_name);
    if (!empty($vocabulary)) {
      $query_level1 = db_select('taxonomy_term_data', 't1');
      $query_level1->join('taxonomy_term_hierarchy', 'tth1', 't1.tid = tth1.tid');
      $query_level1->fields('t1', array('tid'));
      $query_level1->condition('tth1.parent', $parent);

      $query_level2 = db_select('taxonomy_term_data', 't2');
      $query_level2->join('taxonomy_term_hierarchy', 'tth2', 't2.tid = tth2.tid');
      $query_level2->fields('t2', array('tid'));
      $query_level2->condition('tth2.parent', $query_level1, 'IN');

      $query_level3 = db_select('taxonomy_term_data', 't3');
      $query_level3->join('taxonomy_term_hierarchy', 'tth3', 't3.tid = tth3.tid');
      $query_level3->fields('t3', array('tid', 'name'));
      $query_level3->condition('tth3.parent', $query_level2, 'IN');

      if (!empty($term_name)) {
        $query_level3->condition('t3.name', db_like($term_name) . '%', 'LIKE');
      }

      $results = $query_level3->range(0, 4)
        ->execute();

      foreach ($results as $key => $term) {
        $key = $key_name ? $term->name : $term->tid;
        $terms[$key] = $term->name;
      }
    }
    return $terms;
  }

  /**
   * @breaf Retrieve data from user instead of doing a complete loading of the
   *        user entity
   * @param $uid the uid of the user that we need some information
   * @param $data the field that we want to retrieve
   *
   * @return the user's data
   */
  public static function getUserData($uid, $data) {
    //Select query
    $query = db_select('users', 'u');
    $query->fields('u', $data);
    $query->leftJoin('file_managed', 'fm', 'u.picture = fm.fid'); // User picture could be 0
    $query->fields('fm', array('uri'));
    $query->condition('u.uid', $uid);

    return $query->execute()->fetchObject();
  }

  public static function getUserDataByName($name, $data) {
    //Select query
    $query = db_select('users', 'u');
    $query->fields('u', $data);
    $query->leftJoin('file_managed', 'fm', 'u.picture = fm.fid'); // User picture could be 0
    $query->fields('fm', array('uri'));
    $query->condition('u.name', $name);

    return $query->execute()->fetchObject();
  }

public static function getNodeTitle($nid) {
    if (module_exists('title')) {
      global $language;
      // If title module exist get the translated title:
      $query = db_select('field_data_title_field', 'tf');
      $query->addfield('tf', 'title_field_value', 'title');
      $query->condition('tf.entity_id', $nid);
      $query->condition('tf.language', $language->language);
    }
    else {
      // Get the default drupal title property:
      $query = db_select('node', 'n');
      $query->fields('n', array('title'));
      $query->condition('n.nid', $nid);
    }

    return $query->execute()->fetchObject();
  }

  /**
   * @brief Retrieve a EntityfieldQUery configured to get the content publish by a user
   * @return new EntityFieldQuery();
   */
  public static function getPublishedContent($uid, $bundle, $type = 'node') {

    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', $type)
      ->propertyCondition('status', 1)
      ->propertyCondition('uid', $uid)
      ->addTag('node_access');

    if (isset($bundle)) {
      $query->entityCondition('bundle', $bundle);
    }

    return $query;
  }

  /**
   * @brief Retrieve the number of people that a user given is following
   * @return The number of followings
   */
  public static function getFollowingCount($uid) {
    //Get the fid for follow_user machine name
    $query = db_select('flag', 'f');
    $query->fields('f', array('fid'));
    $query->condition('name', 'follow_user');
    $fid = $query->execute()->fetchField(0);

    $query = db_select('flagging', 'f');
    $query->fields('f', array('flagging_id'));
    $query->condition('fid', $fid);
    $query->condition('uid', $uid);

    $count = $query->execute()->rowCount();

    return $count;
  }

  /**
   * @brief Retrieve the number of followers of a given user
   * @return The number of followers
   */
  public static function getFollowersCount($uid) {
    //Get the fid for follow_user machine name
    $query = db_select('flag', 'f');
    $query->fields('f', array('fid'));
    $query->condition('name', 'follow_user');
    $fid = $query->execute()->fetchField(0);

    $query = db_select('flagging', 'f');
    $query->fields('f', array('flagging_id'));
    $query->condition('fid', $fid);
    $query->condition('entity_id', $uid);

    $count = $query->execute()->rowCount();

    return $count;
  }

  /**
   * @brief This function return the list of uids of all the user with role
   * 'administrator'
   * @return The list of uids
   */
  public static function getUidsByRole($role) {
    //Select the uids
    $query = db_select('users', 'u');
    $query->fields('u', array('uid'));
    $query->leftJoin('users_roles', 'ur', 'ur.uid = u.uid');
    $query->leftJoin('role', 'r', 'r.rid = ur.rid ');
    $query->condition('r.name', $role);

    $results = $query->execute()->fetchAllAssoc('uid');

    return $results;
  }


}
