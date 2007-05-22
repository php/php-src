--TEST--
Sybase-CT select LONG_MAX / LONG_MIN
--SKIPIF--
<?php require('skipif.inc'); ?>
--FILE--
<?php
/* This file is part of PHP test framework for ext/sybase_ct
 *
 * $Id$ 
 */

  require('test.inc');

  define('LONG_MAX', is_int(2147483648) ? 9223372036854775807 : 2147483647);
  define('LONG_MIN', -LONG_MAX - 1);

  $db= sybase_connect_ex();
  var_dump(sybase_select_db(TEMPDB, $db));
  
  // Create table and insert some values
  var_dump(sybase_query('create table test_long (value numeric(20, 0))'));
  var_dump(sybase_query('insert test_long values ('.(LONG_MAX - 1).')'));
  var_dump(sybase_query('insert test_long values ('.(LONG_MAX).')'));
  var_dump(sybase_query('insert test_long values ('.(LONG_MAX + 1).')'));
  var_dump(sybase_query('insert test_long values ('.(LONG_MIN + 1).')'));
  var_dump(sybase_query('insert test_long values ('.(LONG_MIN).')'));
  var_dump(sybase_query('insert test_long values ('.(LONG_MIN - 1).')'));
  
  // Select values
  var_dump(sybase_select_ex($db, 'select value from test_long'));
  
  // Drop table
  var_dump(sybase_query('drop table test_long'));
          
  sybase_close($db);
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
>>> Query: select value from test_long
<<< Return: resource
array(6) {
  [0]=>
  array(1) {
    ["value"]=>
    int(%s)
  }
  [1]=>
  array(1) {
    ["value"]=>
    int(%s)
  }
  [2]=>
  array(1) {
    ["value"]=>
    float(%s)
  }
  [3]=>
  array(1) {
    ["value"]=>
    int(-%s)
  }
  [4]=>
  array(1) {
    ["value"]=>
    int(-%s)
  }
  [5]=>
  array(1) {
    ["value"]=>
    float(-%s)
  }
}
bool(true)
