--TEST--
Test imap_fetchheader() function : usage variations - $msg_no argument
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php
/* Prototype  : string imap_fetchheader(resource $stream_id, int $msg_no [, int $options])
 * Description: Get the full unfiltered header for a message
 * Source code: ext/imap/php_imap.c
 */

/*
 * Pass different integers and strings as $msg_no argument
 * to test behaviour of imap_fetchheader()
 */

echo "*** Testing imap_fetchheader() : usage variations ***\n";

require_once(__DIR__.'/imap_include.inc');

$stream_id = setup_test_mailbox('', 3, $mailbox, 'notSimple'); // set up temp mailbox with 3 msgs

$sequences = array (0,     4, // out of range
                    '1,3', '1:3', // message sequences instead of numbers
                    );

foreach($sequences as $msg_no) {
	echo "\n-- \$msg_no is $msg_no --\n";
	var_dump($overview = imap_fetchheader($stream_id, $msg_no));
	if (!$overview) {
		echo imap_last_error() . "\n";
	}
}

// clear error stack
imap_errors();
?>
===DONE===
--CLEAN--
<?php
require_once(__DIR__.'/clean.inc');
?>
--EXPECTF--
*** Testing imap_fetchheader() : usage variations ***
Create a temporary mailbox and add 3 msgs
.. mailbox '{%s}%s' created

-- $msg_no is 0 --

Warning: imap_fetchheader(): Bad message number in %s on line %d
bool(false)


-- $msg_no is 4 --

Warning: imap_fetchheader(): Bad message number in %s on line %d
bool(false)


-- $msg_no is 1,3 --

Notice: A non well formed numeric value encountered in %s on line %d
string(%d) "From: foo@anywhere.com
Subject: Test msg 1
To: %s
MIME-Version: 1.0
Content-Type: MULTIPART/mixed; BOUNDARY="%s"

"

-- $msg_no is 1:3 --

Notice: A non well formed numeric value encountered in %s on line %d
string(%d) "From: foo@anywhere.com
Subject: Test msg 1
To: %s
MIME-Version: 1.0
Content-Type: MULTIPART/mixed; BOUNDARY="%s"

"
===DONE===
