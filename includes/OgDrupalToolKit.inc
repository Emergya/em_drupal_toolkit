<?php

/**
 * @filesize( )
 */

class OgDrupalToolKit {

  /**
   * @brief Get the number of members of a group.
   * @details Get the number of members of a group.
   * @return Members count.
   */
  public static function countMemberships($entity_type, $gid) {
    $query = new EntityFieldQuery();
    $result = $query
      ->entityCondition('entity_type', 'og_membership')
      // Type can be "node" or "user"
      ->propertyCondition('entity_type', $entity_type, '=')
      // This is the group ID
      ->propertyCondition('gid', $gid, '=')
      // Get only the active members
      ->propertyCondition('state', OG_STATE_ACTIVE, '=')
      ->count()
      ->execute();

    return $result;
  }

  public static function getUsersByRoleName($group_type, $bundle, $gid, $role_name) {
    $query_gid = og_is_group_default_access($group_type, $gid) ? 0 : $gid;

    $query = db_select('og_users_roles', 'ogur');
    $query->innerJoin('og_role', 'ogr', 'ogur.rid = ogr.rid');
    $query->innerJoin('og_membership', 'ogm', 'ogur.uid = ogm.etid');

    $rids = $query
      ->fields('ogur', array('rid', 'uid'))
      ->fields('ogr', array('name'))
      ->condition('ogr.group_type', $group_type, '=')
      ->condition('ogr.group_bundle', $bundle, '=')
      ->condition('ogr.gid', $query_gid, '=')
      ->condition('ogr.name', $role_name, '=')
      ->condition('ogur.gid', $gid, '=')
      ->condition('ogm.state', OG_STATE_ACTIVE, '=')
      ->orderBy('rid')
      ->execute()
      ->fetchAllkeyed();
      return $query->execute();
  }

  /**
   * @brief Get the total number of contents of a Group.
   *        - Entity_type : Entity type
   *        - Bundle: Bundle
   *        - Gid: Gid to search by
   * @return Int with the total count.
   */
  public static function countContentsByGroup($entity_type, $bundle, $gid) {
    $query = new EntityFieldQuery();
    return $query->entityCondition('entity_type', $entity_type)
                ->entityCondition('bundle', $bundle)
                ->fieldCondition('og_group_ref', 'target_id', $gid, '=')
                ->count()
                ->execute();
  }
}

