--TEST--
Sybase-CT application name
--SKIPIF--
<?php require('skipif.inc'); ?>
--FILE--
<?php
/* This file is part of PHP test framework for ext/sybase_ct
 *
 * $Id$ 
 */

  require('test.inc');
  
  // {{{ bool compare_string(string expect, string actual)
  //     Compares expect to a trimmed version of actual
  function compare_string($expect, $actual) {
    $trimmed= rtrim($actual, ' ');
    return (0 == strncmp($expect, $trimmed, strlen($trimmed)));
  }
  // }}}

  $program_name= basename(__FILE__, '.php');
  $hostname= 'php.net';
  ini_set('sybct.hostname', $hostname);
  
  $db= sybase_connect_ex(NULL, $program_name);
  $r= sybase_select_ex($db, '
    select 
      hostname, 
      program_name
    from 
      master..sysprocesses
    where
      program_name = "'.$program_name.'"'
  );
  
  var_dump(
    $r,
    compare_string($program_name, $r[0]['program_name']),
    compare_string($hostname, $r[0]['hostname'])
  );
 
  sybase_close($db);
?>
--EXPECTF--
>>> Query: 
    select 
      hostname, 
      program_name
    from 
      master..sysprocesses
    where
      program_name = "test_appname"
<<< Return: resource
array(1) {
  [0]=>
  array(2) {
    ["hostname"]=>
    string(10) "php.net%s"
    ["program_name"]=>
    string(16) "test_appname%s"
  }
}
bool(true)
bool(true)
