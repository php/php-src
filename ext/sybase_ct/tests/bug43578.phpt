--TEST--
Sybase-CT bug #43578 (Incurred fault #6 - if returned textfield ist empty)
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
  
  // Create a temporary table and fill it with test values
  var_dump(sybase_query('
    create table #Resource (
      Resource_ID int,
      DC_Rights text null
    )
  ', $db));
  var_dump(sybase_query('insert into #Resource values (123, NULL)', $db));
  var_dump(sybase_query('insert into #Resource values (124, "")', $db));

  // Select non-existant
  var_dump(sybase_select_ex($db, 'select DC_Rights from #Resource where Resource_ID = 122'));

  // Select null
  var_dump(sybase_select_ex($db, 'select DC_Rights from #Resource where Resource_ID = 123'));

  // Select empty
  var_dump(sybase_select_ex($db, 'select DC_Rights from #Resource where Resource_ID = 124'));
  
  // Clean up and close connection
  var_dump(sybase_query('drop table #Resource', $db));
  sybase_close($db);
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
>>> Query: select DC_Rights from #Resource where Resource_ID = 122
<<< Return: resource
array(0) {
}
>>> Query: select DC_Rights from #Resource where Resource_ID = 123
<<< Return: resource
array(1) {
  [0]=>
  array(1) {
    ["DC_Rights"]=>
    NULL
  }
}
>>> Query: select DC_Rights from #Resource where Resource_ID = 124
<<< Return: resource
array(1) {
  [0]=>
  array(1) {
    ["DC_Rights"]=>
    string(1) " "
  }
}
bool(true)
