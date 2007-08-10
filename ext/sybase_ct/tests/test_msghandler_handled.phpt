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
  
  // {{{ bool sybase_msg_handler(int msgnumber, int severity, int state, int line, string text)
  //     Handles server messages
  function user_message_handler($msgnumber, $severity, $state, $line, $text) {
    switch ($msgnumber) {
      case 0:       // print message
        printf("!!! %s\n", trim($text));
        return;
        
      case 174:     // The function 'GETDATE' requires 0 arguments.
      case 11021:   // Function GETDATE invoked with wrong number or type of argument(s)
        printf("*** Caught '%s'\n", trim($text));
        return;
    }
    printf("Cannot handle message #%d\n", $msgnumber);
    return FALSE;   // Indicate we can't handle this
  }
  // }}}

  sybase_set_messagehandler_ex('user_message_handler');    
  $db= sybase_connect_ex();
  sybase_min_server_severity(0);
  
  // These are handled
  var_dump(sybase_select_ex($db, 'select getdate(NULL)'));
  var_dump(sybase_select_ex($db, 'print "Hi"'));
  
  // These aren't
  var_dump(sybase_select_ex($db, 'use NULL'));
  var_dump(sybase_select_ex($db, 'select convert(datetime, "notadate")'));
  
  sybase_close($db);
?>
--EXPECTF--
bool(true)
>>> Query: select getdate(NULL)
*** Caught '%s'
<<< Return: boolean
bool(false)
>>> Query: print "Hi"
!!! Hi
<<< Return: boolean
bool(true)
>>> Query: use NULL
Cannot handle message #156

Warning: sybase_query(): Sybase:  Server message:  Incorrect syntax near the keyword 'NULL'.
 (severity 15, procedure N/A) in %s on line %d
<<< Return: boolean
bool(false)
>>> Query: select convert(datetime, "notadate")
Cannot handle message #249

Warning: sybase_query(): Sybase:  Server message:  Syntax error during explicit conversion of VARCHAR value 'notadate' to a DATETIME field.
 (severity 16, procedure N/A) in %s on line %d
<<< Return: boolean
bool(false)
