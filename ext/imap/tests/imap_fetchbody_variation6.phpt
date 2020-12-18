--TEST--
Test imap_fetchbody() function : usage variations - $message_num arg
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php
/*
 * Pass different integers, strings, msg sequences and msg UIDs as $message_num argument
 * to test behaviour of imap_fetchbody()
 */

echo "*** Testing imap_fetchbody() : usage variations ***\n";

require_once(__DIR__.'/setup/imap_include.inc');

//Initialise required variables
$stream_id = setup_test_mailbox('imapfetchbodyvar6', 3); // set up temp mailbox with  simple msgs
$section = 1;

$sequences = [0, /* out of range */ 4, 1];

foreach($sequences as $message_num) {
    echo "\n-- \$message_num is $message_num --\n";
    try {
        var_dump(imap_fetchbody($stream_id, $message_num, $section));
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
}
?>
--CLEAN--
<?php
$mailbox_suffix = 'imapfetchbodyvar6';
require_once(__DIR__.'/setup/clean.inc');
?>
--EXPECTF--
*** Testing imap_fetchbody() : usage variations ***
Create a temporary mailbox and add 3 msgs
New mailbox created

-- $message_num is 0 --
imap_fetchbody(): Argument #2 ($message_num) must be greater than 0

-- $message_num is 4 --

Warning: imap_fetchbody(): Bad message number in %s on line %d
bool(false)

-- $message_num is 1 --
string(%d) "1: this is a test message, please ignore
newline%r\R?%r"
