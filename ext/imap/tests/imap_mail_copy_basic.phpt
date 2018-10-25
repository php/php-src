--TEST--
Test imap_mail_copy() function : basic functionality
--CREDITS--
Olivier Doucet
--SKIPIF--
<?php
require_once(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php
/* Prototype  : bool imap_mail_copy  ( resource $imap_stream  , string $msglist  , string $mailbox  [, int $options = 0  ] )
 * Description: Copies mail messages specified by msglist  to specified mailbox.
 * Source code: ext/imap/php_imap.c
 */

echo "*** Testing imap_mail_copy() : basic functionality ***\n";

require_once(dirname(__FILE__).'/imap_include.inc');


echo "Create a new mailbox for test\n";
$imap_stream = setup_test_mailbox("", 1);
if (!is_resource($imap_stream)) {
	exit("TEST FAILED: Unable to create test mailbox\n");
}

$check = imap_check($imap_stream);
echo "Msg Count in new mailbox: ". $check->Nmsgs . "\n";

var_dump(imap_mail_copy($imap_stream, '1', 'INBOX.'.$mailbox_prefix));

imap_close($imap_stream);
?>
===Done===
--CLEAN--
<?php
require_once('clean.inc');
?>
--EXPECTF--
*** Testing imap_mail_copy() : basic functionality ***
Create a new mailbox for test
Create a temporary mailbox and add 1 msgs
.. mailbox '%s' created
Msg Count in new mailbox: 1
bool(true)
===Done===
