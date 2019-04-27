--TEST--
Test imap_body() function : basic functionality
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php
/* Prototype  : string imap_body  ( resource $imap_stream  , int $msg_number  [, int $options  ] )
 * Description: Read the message body.
 * Source code: ext/imap/php_imap.c
 */

echo "*** Testing imap_body() : basic functionality ***\n";

require_once(__DIR__.'/imap_include.inc');

echo "Create a new mailbox for test\n";
$imap_stream = setup_test_mailbox("", 1);
if (!is_resource($imap_stream)) {
	exit("TEST FAILED: Unable to create test mailbox\n");
}

$check = imap_check($imap_stream);
echo "Msg Count in new mailbox: ". $check->Nmsgs . "\n";

// show body for msg 1
var_dump(imap_body($imap_stream, 1));

//Access via FT_UID
var_dump(imap_body($imap_stream, 1, FT_UID));

imap_close($imap_stream);
?>
===Done===
--CLEAN--
<?php
require_once('clean.inc');
?>
--EXPECTF--
*** Testing imap_body() : basic functionality ***
Create a new mailbox for test
Create a temporary mailbox and add 1 msgs
.. mailbox '%s' created
Msg Count in new mailbox: 1
string(%d) "1: this is a test message, please ignore%a"
string(%d) "1: this is a test message, please ignore%a"
===Done===
