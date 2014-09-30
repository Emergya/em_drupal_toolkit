<?php

/**
*
*/
class testOgDrupalToolKit extends DrupalWebTestCase {

  public static function getInfo() {
    return array(
      'name' => 'Og Drupal tool kit test',
      'description' => 'Test the static methods of OgDrupalToolKit class',
      'group' => 'Emergya Drupal Toolkit',
    );
  }

  public function setUp() {
    // Ensure enable og module.
    parent::setUp('og', 'em_drupal_toolkit');

  }

  public function testOgDrupalToolKitMethods() {
    // Set Article Content Type as Group:
    og_create_field(OG_GROUP_FIELD, 'node', 'article');
    // Create a group content:
    $group_settings = array(
      'type' => 'article',
      'title' => $this->randomName(32),
      'body' => array(LANGUAGE_NONE => array(array($this->randomName(64)))),
    );
    $group_node = $this->drupalCreateNode($group_settings);
    $group_wrapper = entity_metadata_wrapper('node', $group_node);
    $group_wrapper->{OG_GROUP_FIELD}->set(1);
    $group_wrapper->save();

    // Set Page Content type as group content:
    og_create_field(OG_AUDIENCE_FIELD, 'node', 'page');
    // Generate content for the group node:
    $content_settings = array(
      'type' => 'article',
      'title' => $this->randomName(32),
      'body' => array(LANGUAGE_NONE => array(array($this->randomName(64)))),
    );
    $content_node = $this->drupalCreateNode($content_settings);
    $content_wrapper = entity_metadata_wrapper('node', $content_node);
    $content_wrapper->{OG_AUDIENCE_FIELD}->set(array($group_node->nid));
    $content_wrapper->save();

    // Test the method countContentsByGroup:
    $contents_count = OgDrupalToolKit::countContentsByGroup('node', 'article', $group_node->nid);
    $this->assertEqual($contents_count, 1, 'countContentsByGroup method works as expected');
  }
}


