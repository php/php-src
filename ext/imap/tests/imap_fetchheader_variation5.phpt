--TEST--
Test imap_fetchheader() function : usage variations - $message_num argument
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php
/*
 * Pass different integers and strings as $message_num argument
 * to test behaviour of imap_fetchheader()
 */

echo "*** Testing imap_fetchheader() : usage variations ***\n";

require_once(__DIR__.'/imap_include.inc');

$stream_id = setup_test_mailbox('', 3, $mailbox, 'notSimple'); // set up temp mailbox with 3 msgs

$sequences = [0, /* out of range */ 4, 1];

foreach($sequences as $message_num) {
    echo "\n-- \$message_num is $message_num --\n";
    try {
        var_dump(imap_fetchheader($stream_id, $message_num));
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
}

// clear error stack
imap_errors();
?>
--CLEAN--
<?php
require_once(__DIR__.'/clean.inc');
?>
--EXPECTF--
*** Testing imap_fetchheader() : usage variations ***
Create a temporary mailbox and add 3 msgs
.. mailbox '{%s}%s' created

-- $message_num is 0 --
imap_fetchheader(): Argument #2 ($message_num) must be greater than 0

-- $message_num is 4 --

Warning: imap_fetchheader(): Bad message number in %s on line %d
bool(false)

-- $message_num is 1 --
string(%d) "From: foo@anywhere.com
Subject: Test msg 1
To: %s
MIME-Version: 1.0
Content-Type: MULTIPART/mixed; BOUNDARY="%s=:%d"

"
