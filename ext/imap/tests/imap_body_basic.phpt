--TEST--
Test imap_body() function : basic functionality
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php
echo "*** Testing imap_body() : basic functionality ***\n";

require_once(__DIR__.'/setup/imap_include.inc');

echo "Create a new mailbox for test\n";
$imap_stream = setup_test_mailbox("imapbodybasic", 1);

$check = imap_check($imap_stream);
echo "Msg Count in new mailbox: ". $check->Nmsgs . "\n";

// show body for msg 1
var_dump(imap_body($imap_stream, 1));

//Access via FT_UID
var_dump(imap_body($imap_stream, 1, FT_UID));

imap_close($imap_stream);
?>
--CLEAN--
<?php
$mailbox_suffix = 'imapbodybasic';
require_once __DIR__.'/setup/clean.inc';
?>
--EXPECTF--
*** Testing imap_body() : basic functionality ***
Create a new mailbox for test
Create a temporary mailbox and add 1 msgs
New mailbox created
Msg Count in new mailbox: 1
string(%d) "1: this is a test message, please ignore
newline%r\R?%r"
string(%d) "1: this is a test message, please ignore
newline%r\R?%r"
