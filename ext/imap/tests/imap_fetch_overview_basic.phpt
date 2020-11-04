--TEST--
Test imap_fetch_overview() function : basic functionality
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php
echo "*** Testing imap_fetch_overview() : basic functionality ***\n";

require_once __DIR__.'/setup/imap_include.inc';

// create a new mailbox and add two new messages to it
$stream_id = setup_test_mailbox('imapfetchoverviewbasic', 2, $mailbox, false);

// get UID for new message
$msg_no = imap_uid($stream_id, 1);
$options = FT_UID;

// Calling imap_fetch_overview() with all possible arguments
echo "\n-- All possible arguments --\n";
$a =  imap_fetch_overview($stream_id, "$msg_no", $options) ;
echo "\n--> Object #1\n";
displayOverviewFields($a[0]);

// Calling imap_fetch_overview() with mandatory arguments
echo "\n-- Mandatory arguments --\n";
$a = imap_fetch_overview($stream_id, '1:2') ;

//first object in array
echo "\n--> Object #1\n";
displayOverviewFields($a[0]);

//Second object in array
echo "\n--> Object #2\n";
displayOverviewFields($a[1]);

imap_close($stream_id);

?>
--CLEAN--
<?php
$mailbox_suffix = 'imapfetchoverviewbasic';
require_once __DIR__.'/setup/clean.inc';
?>
--EXPECTF--
*** Testing imap_fetch_overview() : basic functionality ***
Create a temporary mailbox and add 2 msgs
New mailbox created

-- All possible arguments --

--> Object #1
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

-- Mandatory arguments --

--> Object #1
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

--> Object #2
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
