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
class stdClass {
  %s $name = 'id';
  %s $max_length = 11;
  %s $column_source = '';
  %s $numeric = 1;
  %s $type = 'int';
}
class stdClass {
  %s $name = 'caption';
  %s $max_length = 5;
  %s $column_source = '';
  %s $numeric = 0;
  %s $type = 'string';
}
class stdClass {
  %s $name = 'author';
  %s $max_length = 4;
  %s $column_source = '';
  %s $numeric = 0;
  %s $type = 'string';
}
class stdClass {
  %s $name = 'lastchange';
  %s $max_length = 29;
  %s $column_source = '';
  %s $numeric = 0;
  %s $type = 'datetime';
}
bool(true)
class stdClass {
  %s $name = 'caption';
  %s $max_length = 5;
  %s $column_source = '';
  %s $numeric = 0;
  %s $type = 'string';
}
