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

  echo 'Nonexistant:     '; sybase_set_messagehandler_ex('function_does_not_exist');
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
Nonexistant:     
Warning: sybase_set_message_handler(): First argumented is expected to be a valid callback, 'function_does_not_exist' was given in %s/test.inc on line %d
bool(false)
Static method:   bool(true)
Instance method: bool(true)
Lambda function: bool(true)
Unset:           bool(true)
Incorrect type:  
Warning: sybase_set_message_handler(): First argumented is expected to be either NULL, an array or string, integer given in %s/test.inc on line %d
bool(false)
Function:        bool(true)
>>> Query: select getdate(NULL)
*** Caught Sybase Server Message #%d [Severity %d, state %d] at line %d
    %s
<<< Return: boolean
bool(false)
