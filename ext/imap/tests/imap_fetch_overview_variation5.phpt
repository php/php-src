--TEST--
Test imap_fetch_overview() function : usage variations - $msg_no argument
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php
/* Prototype  : array imap_fetch_overview(resource $stream_id, int $msg_no [, int $options])
 * Description: Read an overview of the information in the headers
 * of the given message sequence
 * Source code: ext/imap/php_imap.c
 */

/*
 * Pass different sequences/msg numbers as $msg_no argument to test behaviour
 * of imap_fetch_overview()
 */

echo "*** Testing imap_fetch_overview() : usage variations ***\n";
require_once(__DIR__.'/imap_include.inc');

$stream_id = setup_test_mailbox('', 3, $mailbox, 'notSimple'); // set up temp mailbox with 3 msgs

$sequences = array (0,     4,     '4', // out of range
                    '2',   '1,3', '1, 2',
                    '1:3'); // pass uid without setting FT_UID option

foreach($sequences as $msg_no) {
	echo "\n-- \$msg_no is $msg_no --\n";
        $overview = imap_fetch_overview($stream_id, $msg_no);
	if (!$overview) {
		echo imap_last_error() . "\n";
        } else {
		foreach($overview as $ov) {
			echo "\n";
			displayOverviewFields($ov);
       		 }
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
*** Testing imap_fetch_overview() : usage variations ***
Create a temporary mailbox and add 3 msgs
.. mailbox '{%s}%s' created

-- $msg_no is 0 --
Sequence out of range

-- $msg_no is 4 --
Sequence out of range

-- $msg_no is 4 --
Sequence out of range

-- $msg_no is 2 --

size is %d
uid is %d
msgno is 2
recent is %d
flagged is 0
answered is 0
deleted is 0
seen is 0
draft is 0
udate is OK

-- $msg_no is 1,3 --

size is %d
uid is %d
msgno is 1
recent is %d
flagged is 0
answered is 0
deleted is 0
seen is 0
draft is 0
udate is OK

size is %d
uid is %d
msgno is 3
recent is %d
flagged is 0
answered is 0
deleted is 0
seen is 0
draft is 0
udate is OK

-- $msg_no is 1, 2 --
Syntax error in sequence

-- $msg_no is 1:3 --

size is %d
uid is %d
msgno is 1
recent is %d
flagged is 0
answered is 0
deleted is 0
seen is 0
draft is 0
udate is OK

size is %d
uid is %d
msgno is 2
recent is %d
flagged is 0
answered is 0
deleted is 0
seen is 0
draft is 0
udate is OK

size is %d
uid is %d
msgno is 3
recent is %d
flagged is 0
answered is 0
deleted is 0
seen is 0
draft is 0
udate is OK
===DONE===
