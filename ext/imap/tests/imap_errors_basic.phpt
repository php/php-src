--TEST--
Test imap_errors() function : basic functionality 
--SKIPIF--
<?php 
require_once(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php
/* Prototype  : array imap_errors  ( void  )
 * Description: Returns all of the IMAP errors that have occured.
 * Source code: ext/imap/php_imap.c
 */

echo "*** Testing imap_errors() : basic functionality ***\n";
require_once(dirname(__FILE__).'/imap_include.inc');
$password = "bogus"; // invalid password to use in this test 

echo "Issue open with invalid password with normal default number of retries, i.e 3\n";
$mbox = imap_open($default_mailbox, $username, $password, OP_READONLY, 3);

echo "List any errors\n";
var_dump(imap_errors()); 

echo "\n\nIssue open with invalid password with retries == 1\n";
$mbox = imap_open($default_mailbox, $username, $password, OP_READONLY, 1);

echo "List any errors\n";
var_dump(imap_errors()); 
?>
===Done===
--EXPECTF--
*** Testing imap_errors() : basic functionality ***
Issue open with invalid password with normal default number of retries, i.e 3

Warning: imap_open(): Couldn't open stream %s in %s on line %d
List any errors
array(%d) {
  [0]=>
  string(%d) "%s"
  [1]=>
  string(%d) "%s"
  [2]=>
  string(%d) "%a
}


Issue open with invalid password with retries == 1

Warning: imap_open(): Couldn't open stream %s in %s on line %d
List any errors
array(%d) {
  [0]=>
  string(%d) "%a
}
===Done===
