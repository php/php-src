--TEST--
Sybase-CT bug #30312 (sybase_unbuffered_query calls, with free)
--SKIPIF--
<?php require('skipif.inc'); ?>
--FILE--
<?php
/* This file is part of PHP test framework for ext/sybase_ct
 *
 * $Id$
 */

  require('test.inc');
  error_reporting(error_reporting() & !E_NOTICE);       // Suppress notices

  $db= sybase_connect_ex();

  $query= sybase_unbuffered_query('select datepart(yy, getdate())');
  $array= sybase_fetch_row($query);
  var_dump($array[0]);
  sybase_free_result($query);

  $query= sybase_unbuffered_query('select datepart(mm, getdate()), datepart(dd, getdate())');
  $array= sybase_fetch_row($query);
  var_dump($array);
  sybase_free_result($query);
?>
--EXPECTF--
int(%d)
array(2) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
}
