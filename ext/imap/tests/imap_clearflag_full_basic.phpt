--TEST--
Test imap_clearflag_full() function : basic functionality
--SKIPIF--
<?php
require_once(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php
/* Prototype  : bool imap_clearflag_full  ( resource $imap_stream  , string $sequence  , string $flag  [, string $options  ] )
 * Description: Clears flags on messages.
 * Source code: ext/imap/php_imap.c
 */

echo "*** Testing imap_clearflag_full() : basic functionality ***\n";

require_once(dirname(__FILE__).'/imap_include.inc');

echo "Create a new mailbox for test\n";
$imap_stream = setup_test_mailbox("", 10);
if (!is_resource($imap_stream)) {
	exit("TEST FAILED: Unable to create test mailbox\n");
}

$check = imap_check($imap_stream);
echo "Initial msg count in new_mailbox : ". $check->Nmsgs . "\n";

echo "Set some flags\n";
var_dump(imap_setflag_full($imap_stream, "1,3", "\\Seen \\Answered"));
var_dump(imap_setflag_full($imap_stream, "2,4", "\\Answered"));
var_dump(imap_setflag_full($imap_stream, "5,7", "\\Flagged \\Deleted"));
var_dump(imap_setflag_full($imap_stream, "6,8", "\\Deleted"));
var_dump(imap_setflag_full($imap_stream, "9,10", "\\Draft \\Flagged"));

var_dump(imap_search($imap_stream, "SEEN"));
var_dump(imap_search($imap_stream, "ANSWERED"));
var_dump(imap_search($imap_stream, "FLAGGED"));
var_dump(imap_search($imap_stream, "DELETED"));

var_dump(imap_clearflag_full($imap_stream, "1,4", "\\Answered"));
var_dump(imap_clearflag_full($imap_stream, "5,6,7,8", "\\Deleted"));
var_dump(imap_clearflag_full($imap_stream, "9", "\\Flagged"));

var_dump(imap_search($imap_stream, "SEEN"));
var_dump(imap_search($imap_stream, "ANSWERED"));
var_dump(imap_search($imap_stream, "FLAGGED"));
var_dump(imap_search($imap_stream, "DELETED"));

imap_close($imap_stream);
?>
===Done===
--CLEAN--
<?php
require_once('clean.inc');
?>
--EXPECTF--
*** Testing imap_clearflag_full() : basic functionality ***
Create a new mailbox for test
Create a temporary mailbox and add 10 msgs
.. mailbox '{%s}%s' created
Initial msg count in new_mailbox : 10
Set some flags
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(3)
}
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
}
array(4) {
  [0]=>
  int(5)
  [1]=>
  int(7)
  [2]=>
  int(9)
  [3]=>
  int(10)
}
array(4) {
  [0]=>
  int(5)
  [1]=>
  int(6)
  [2]=>
  int(7)
  [3]=>
  int(8)
}
bool(true)
bool(true)
bool(true)
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(3)
}
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}
array(3) {
  [0]=>
  int(5)
  [1]=>
  int(7)
  [2]=>
  int(10)
}
bool(false)
===Done===
