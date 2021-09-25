--TEST--
Test imap_fetch_overview() function : usage variations - $msg_no argument
--EXTENSIONS--
imap
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php
/*
 * Pass different sequences/msg numbers as $msg_no argument to test behaviour
 * of imap_fetch_overview()
 */

echo "*** Testing imap_fetch_overview() : usage variations ***\n";
require_once __DIR__.'/setup/imap_include.inc';

$stream_id = setup_test_mailbox('imapfetchoverviewvar5', 3, $mailbox, false); // set up temp mailbox with 3 msgs

$sequences = [
    0,
    4, // out of range
    '4', // out of range
    '2',
    '1,3',
    '1, 2',
    '1:3', // pass uid without setting FT_UID option
];

foreach ($sequences as $msg_no) {
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
--CLEAN--
<?php
$mailbox_suffix = 'imapfetchoverviewvar5';
require_once(__DIR__.'/setup/clean.inc');
?>
--EXPECTF--
*** Testing imap_fetch_overview() : usage variations ***
Create a temporary mailbox and add 3 msgs
New mailbox created

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
