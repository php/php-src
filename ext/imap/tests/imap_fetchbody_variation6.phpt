--TEST--
Test imap_fetchbody() function : usage variations - $msg_no arg
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php
/*
 * Pass different integers, strings, msg sequences and msg UIDs as $msg_no argument
 * to test behaviour of imap_fetchbody()
 */

echo "*** Testing imap_fetchbody() : usage variations ***\n";

require_once(__DIR__.'/imap_include.inc');

//Initialise required variables
$stream_id = setup_test_mailbox('', 3); // set up temp mailbox with  simple msgs
$section = 1;

$sequences = [0, /* out of range */ 4, 1];

foreach($sequences as $msg_no) {
    echo "\n-- \$msg_no is $msg_no --\n";
    var_dump($overview = imap_fetchbody($stream_id, $msg_no, $section));
    if (!$overview) {
        echo imap_last_error() . "\n";
    }
}
?>
--CLEAN--
<?php
require_once(__DIR__.'/clean.inc');
?>
--EXPECTF--
*** Testing imap_fetchbody() : usage variations ***
Create a temporary mailbox and add 3 msgs
.. mailbox '{%s}%s' created

-- $msg_no is 0 --

Warning: imap_fetchbody(): Bad message number in %s on line %d
bool(false)


-- $msg_no is 4 --

Warning: imap_fetchbody(): Bad message number in %s on line %d
bool(false)


-- $msg_no is 1 --
string(42) "1: this is a test message, please ignore
"
