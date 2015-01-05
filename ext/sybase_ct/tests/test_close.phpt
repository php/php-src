--TEST--
Sybase-CT close
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
  var_dump($db);
  sybase_close($db);
  var_dump($db);
  var_dump(sybase_query('select getdate()', $db));
?>
--EXPECTF--
resource(%d) of type (sybase-ct link)
resource(%d) of type (Unknown)

Warning: sybase_query(): %d is not a valid Sybase-Link resource in %s on line %d
bool(false)
