--TEST--
Sybase-CT connection-based server message handler
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
  var_dump(sybase_set_message_handler('sybase_msg_handler', $db));
  var_dump(sybase_select_ex($db, 'select getdate(NULL)'));
  sybase_close($db);
?>
--EXPECTF--
resource(%d) of type (sybase-ct link)
bool(true)
>>> Query: select getdate(NULL)
*** Caught Sybase Server Message #%d [Severity %d, state %d] at line %d
    %s
<<< Return: boolean
bool(false)
