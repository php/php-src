--TEST--
Sybase-CT server message handler
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

  echo 'Nonexistent:     '; sybase_set_messagehandler_ex('function_does_not_exist');
  echo 'Static method:   '; sybase_set_messagehandler_ex(array('sybase', 'static_handler'));
  echo 'Instance method: '; sybase_set_messagehandler_ex(array(new sybase(), 'handler'));
  echo 'Lambda function: '; sybase_set_messagehandler_ex(create_function('', 'return FALSE;'));
  echo 'Unset:           '; sybase_set_messagehandler_ex(NULL);
  echo 'Incorrect type:  '; sybase_set_messagehandler_ex(1);
  echo 'Function:        '; sybase_set_messagehandler_ex('sybase_msg_handler');

  var_dump(sybase_select_ex($db, 'select getdate(NULL)'));
  sybase_close($db);
?>
--EXPECTF--
Nonexistent:     
Warning: sybase_set_message_handler() expects parameter 1 to be a valid callback, function 'function_does_not_exist' not found or invalid function name in %stest.inc on line %d
NULL
Static method:   bool(true)
Instance method: bool(true)
Lambda function: bool(true)
Unset:           bool(true)
Incorrect type:  
Warning: sybase_set_message_handler() expects parameter 1 to be a valid callback, no array or string given in %stest.inc on line %d
NULL
Function:        bool(true)
>>> Query: select getdate(NULL)
*** Caught Sybase Server Message #%d [Severity %d, state %d] at line %d
    %s
<<< Return: boolean
bool(false)
