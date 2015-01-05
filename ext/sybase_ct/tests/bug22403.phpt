--TEST--
Sybase-CT bug #22403 (crash when executing a stored procedure without parameters)
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
  sybase_set_message_handler('sybase_msg_handler');
  error_reporting(error_reporting() & !E_NOTICE);       // Suppress notices

  // Check if stored procedure already exists
  $sp_name= 'phpt_bug22403';
  var_dump(sybase_select_db(TEMPDB, $db));
  if (!sybase_select_single($db, 'select object_id("'.$sp_name.'")')) {
    echo "Stored procedure {$sp_name} not found, creating\n";
    var_dump(sybase_query('
      create proc '.$sp_name.' (@param int)
      as
      begin
        select @param
        select @param + 1
        return @param
      end
    '));
  } else {
    echo "Stored procedure {$sp_name} found, using\n";
    var_dump(TRUE);
  }
  
  // These don't work
  var_dump(sybase_select_ex($db, 'exec '.$sp_name));
  var_dump(sybase_select_ex($db, 'exec '.$sp_name.' "foo"'));
  var_dump(sybase_select_ex($db, 'exec does_not_exist'));

  // These do
  var_dump(sybase_select_ex($db, 'exec '.$sp_name.' NULL'));
  var_dump(sybase_select_ex($db, 'exec '.$sp_name.' 1'));

  // Clean up after ourselves
  var_dump(sybase_query('drop proc '.$sp_name));

  sybase_close($db);
?>
--EXPECTF--
bool(true)
Stored procedure %s
bool(true)
>>> Query: exec %s
*** Caught Sybase Server Message #201 [Severity 16, state 2] at line 0
    %s
<<< Return: boolean
bool(false)
>>> Query: exec %s "foo"
*** Caught Sybase Server Message #257 [Severity 16, state 1] at line 0
    %s
<<< Return: boolean
bool(false)
>>> Query: exec does_not_exist
*** Caught Sybase Server Message #2812 [Severity 16, state %d] at line 1
    %s
<<< Return: boolean
bool(false)
>>> Query: exec %s NULL
<<< Return: resource
array(1) {
  [0]=>
  array(1) {
    ["computed"]=>
    NULL
  }
}
>>> Query: exec %s 1
<<< Return: resource
array(1) {
  [0]=>
  array(1) {
    ["computed"]=>
    int(1)
  }
}
bool(true)
