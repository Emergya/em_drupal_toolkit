<?php

/**
 * @file File with the Class definition OgDrupalToolKit.
 */

/**
 * Contains several methods to be used with Organic groups module.
 *
 * @package em_drupal_toolkit
 */
class OgDrupalToolKit {

  /**
   * Count the number of memberships of a concrente Group.
   *
   * Returns the number of members of a concrete group with ACTIVE status.
   *
   * @param  string $entity_type Og membership type, normally 'user'.
   * @param  int $gid Group id to check.
   * @return int Membership count
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

  /**
   * Get the users members with a certain role in a group.
   *
   * Return a Query object with info about users with certain role in a group.
   *
   * @param  string $group_type Group type to check, generally 'node'.
   * @param  string $bundle     Bundle group to check, generally ToC group name.
   * @param  int $gid Group entity id.
   * @param  string $role_name Role to check.
   * @return QueryObject that implements DatabaseStatementInterface with the result.
   */
  public static function getUsersByRoleName($group_type, $bundle, $gid, $role_name) {
    $query_gid = og_is_group_default_access($group_type, $gid) ? 0 : $gid;

    $query = db_select('og_users_roles', 'ogur');
    $query->innerJoin('og_role', 'ogr', 'ogur.rid = ogr.rid');
    $query->innerJoin('og_membership', 'ogm', 'ogur.uid = ogm.etid');

    $query->fields('ogur', array('rid', 'uid'))
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
   * Get the total number of contents of a Group.
   *
   * @param  string $entity_type Group type to check, generally 'node'.
   * @param  string $bundle Bundle group to check, generally ToC group name.
   * @param  int $gid Group entity id.
   * @return Int Total content count.
   */
  public static function countContentsByGroup($entity_type, $bundle, $gid) {
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', $entity_type)
      ->entityCondition('bundle', $bundle)
      ->fieldCondition('og_group_ref', 'target_id', $gid, '=')
      ->count();

    return $query->execute();
  }
}

