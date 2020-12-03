--TEST--
imap_setflag_full() passing a unique ID
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php

require_once(__DIR__.'/setup/imap_include.inc');

$imap_mail_box = setup_test_mailbox_for_uid_tests("imapsetflagfulluid");

/* This works on the assumption that UID message 3 to 6 inclusive are deleted. */

// Testing individual entry
imap_setflag_full($imap_mail_box, '8', '\Answered', ST_UID);

// Testing multiple entries entry
imap_setflag_full($imap_mail_box, '7,10', '\Deleted', ST_UID);

// Testing entry range
imap_setflag_full($imap_mail_box, '7:9', '\Flagged', ST_UID);

// Testing entry range invalid
var_dump(imap_setflag_full($imap_mail_box, '4:9', '\Seen', ST_UID));


echo 'ALL: ';
var_dump(imap_search($imap_mail_box, 'ALL'));
echo 'ALL (with UID correspondance): ';
var_dump(imap_search($imap_mail_box, 'ALL', SE_UID));
echo 'ANSWERED: ';
var_dump(imap_search($imap_mail_box, 'ANSWERED'));
echo 'DELETED: ';
var_dump(imap_search($imap_mail_box, 'DELETED'));
echo 'FLAGGED: ';
var_dump(imap_search($imap_mail_box, 'FLAGGED'));
echo 'SEEN: ';
var_dump(imap_search($imap_mail_box, 'SEEN'));

imap_close($imap_mail_box);

?>
--CLEAN--
<?php
$mailbox_suffix = 'imapsetflagfulluid';
require_once(__DIR__ . '/setup/clean.inc');
?>
--EXPECT--
Create a temporary mailbox and add 10 msgs
New mailbox created
Delete 4 messages for Unique ID generation
bool(true)
ALL: array(6) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
  [5]=>
  int(6)
}
ALL (with UID correspondance): array(6) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(7)
  [3]=>
  int(8)
  [4]=>
  int(9)
  [5]=>
  int(10)
}
ANSWERED: array(1) {
  [0]=>
  int(4)
}
DELETED: array(2) {
  [0]=>
  int(3)
  [1]=>
  int(6)
}
FLAGGED: array(3) {
  [0]=>
  int(3)
  [1]=>
  int(4)
  [2]=>
  int(5)
}
SEEN: array(3) {
  [0]=>
  int(3)
  [1]=>
  int(4)
  [2]=>
  int(5)
}
