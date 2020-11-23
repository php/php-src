--TEST--
Test imap_clearflag_full() function : basic functionality
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php
echo "*** Testing imap_clearflag_full() : basic functionality ***\n";

require_once(__DIR__.'/setup/imap_include.inc');

echo "Create a new mailbox for test\n";
$imap_stream = setup_test_mailbox("imapclearflagfullbasic", 10);

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
--CLEAN--
<?php
$mailbox_suffix = 'imapclearflagfullbasic';
require_once(__DIR__ . '/setup/clean.inc');
?>
--EXPECT--
*** Testing imap_clearflag_full() : basic functionality ***
Create a new mailbox for test
Create a temporary mailbox and add 10 msgs
New mailbox created
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
