--TEST--
Test imap_append() function : basic functionality
--SKIPIF--
<?php
require_once(__DIR__. '/setup/skipif.inc');
?>
--FILE--
<?php
echo "*** Testing imap_append() : basic functionality ***\n";

require_once(__DIR__. '/setup/imap_include.inc');

echo "Create a new mailbox for test\n";
$imap_stream = setup_test_mailbox("imapappendbaisc", 0);

$mb_details = imap_mailboxmsginfo($imap_stream);
echo "Add a couple of msgs to the new mailbox\n";
var_dump(imap_append($imap_stream, $mb_details->Mailbox
                   , "From: webmaster@something.com\r\n"
                   . "To: info@something.com\r\n"
                   . "Subject: Test message\r\n"
                   . "\r\n"
                   . "this is a test message, please ignore\r\n"
                   ));

var_dump(imap_append($imap_stream, $mb_details->Mailbox
                   , "From: webmaster@something.com\r\n"
                   . "To: info@something.com\r\n"
                   . "Subject: Another test\r\n"
                   . "\r\n"
                   . "this is another test message, please ignore it too!!\r\n"
                   ));

$check = imap_check($imap_stream);
echo "Msg Count after append : ". $check->Nmsgs . "\n";

echo "List the msg headers\n";
var_dump(imap_headers($imap_stream));

imap_close($imap_stream);
?>
--CLEAN--
<?php
$mailbox_suffix = 'imapappendbaisc';
require_once(__DIR__ . '/setup/clean.inc');
?>
--EXPECTF--
*** Testing imap_append() : basic functionality ***
Create a new mailbox for test
Create a temporary mailbox and add 0 msgs
New mailbox created
Add a couple of msgs to the new mailbox
bool(true)
bool(true)
Msg Count after append : 2
List the msg headers
array(2) {
  [0]=>
  string(%d) "%w%s       1)%s webmaster@something. Test message (%d chars)"
  [1]=>
  string(%d) "%w%s       2)%s webmaster@something. Another test (%d chars)"
}
