--TEST--
Test imap_fetchheader() function : usage variations - $msg_no argument
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php
/*
 * Pass different integers and strings as $msg_no argument
 * to test behaviour of imap_fetchheader()
 */

echo "*** Testing imap_fetchheader() : usage variations ***\n";

require_once(__DIR__.'/imap_include.inc');

$stream_id = setup_test_mailbox('', 3, $mailbox, 'notSimple'); // set up temp mailbox with 3 msgs

$sequences = [0, /* out of range */ 4, 1];

foreach($sequences as $msg_no) {
    echo "\n-- \$msg_no is $msg_no --\n";
    try {
        var_dump(imap_fetchheader($stream_id, $msg_no));
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

-- $msg_no is 0 --
imap_fetchheader(): Argument #2 ($msg_no) must be greater than 0

-- $msg_no is 4 --

Warning: imap_fetchheader(): Bad message number in %s on line %d
bool(false)

-- $msg_no is 1 --
string(%d) "From: foo@anywhere.com
Subject: Test msg 1
To: webmaster@something.com
MIME-Version: 1.0
Content-Type: MULTIPART/mixed; BOUNDARY="%s=:%d"

"
