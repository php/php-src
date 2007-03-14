--TEST--
Sybase-CT sybase_field_* functions
--SKIPIF--
<?php require('skipif.inc'); ?>
--FILE--
<?php
/* This file is part of PHP test framework for ext/sybase_ct
 *
 * $Id$ 
 */

  require('test.inc');

  $db= sybase_connect_ex();

  // Issue a query
  $q= sybase_unbuffered_query('select
    1 as "id",
    "Hello" as "caption",
    "timm" as "author",
    getdate() as "lastchange"
  ', $db, FALSE);
  var_dump($q);

  var_dump(sybase_num_fields($q));

  // Go through each field, dumping it
  while ($field= sybase_fetch_field($q)) {
    var_export($field); echo "\n";
  }
  
  // Seek to the second field and fetch it
 var_dump(sybase_field_seek($q, 1));
 var_export(sybase_fetch_field($q)); echo "\n";

 sybase_close($db);
?>
--EXPECTF--
resource(%d) of type (sybase-ct result)
int(4)
stdClass::__set_state(array(
   'name' => 'id',
   'max_length' => 11,
   'column_source' => '',
   'numeric' => 1,
   'type' => 'int',
))
stdClass::__set_state(array(
   'name' => 'caption',
   'max_length' => 5,
   'column_source' => '',
   'numeric' => 0,
   'type' => 'string',
))
stdClass::__set_state(array(
   'name' => 'author',
   'max_length' => 4,
   'column_source' => '',
   'numeric' => 0,
   'type' => 'string',
))
stdClass::__set_state(array(
   'name' => 'lastchange',
   'max_length' => 29,
   'column_source' => '',
   'numeric' => 0,
   'type' => 'datetime',
))
bool(true)
stdClass::__set_state(array(
   'name' => 'caption',
   'max_length' => 5,
   'column_source' => '',
   'numeric' => 0,
   'type' => 'string',
))
