--TEST--
imap_setflag_full() basic test
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php

require_once(__DIR__.'/setup/imap_include.inc');

$imap_mail_box = setup_test_mailbox("imapsetflagfullbasic", 10);

// Testing individual entry
imap_setflag_full($imap_mail_box, '1', '\Answered');

// Testing multiple entries entry
imap_setflag_full($imap_mail_box, '2,7', '\Deleted');

// Testing entry range
imap_setflag_full($imap_mail_box, '3:5', '\Flagged');

echo 'ALL: ';
var_dump(imap_search($imap_mail_box, 'ALL'));
echo 'ANSWERED: ';
var_dump(imap_search($imap_mail_box, 'ANSWERED'));
echo 'DELETED: ';
var_dump(imap_search($imap_mail_box, 'DELETED'));
echo 'FLAGGED: ';
var_dump(imap_search($imap_mail_box, 'FLAGGED'));

imap_close($imap_mail_box);

?>
--CLEAN--
<?php
$mailbox_suffix = 'imapsetflagfullbasic';
require_once(__DIR__ . '/setup/clean.inc');
?>
--EXPECT--
Create a temporary mailbox and add 10 msgs
New mailbox created
ALL: array(10) {
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
  [6]=>
  int(7)
  [7]=>
  int(8)
  [8]=>
  int(9)
  [9]=>
  int(10)
}
ANSWERED: array(1) {
  [0]=>
  int(1)
}
DELETED: array(2) {
  [0]=>
  int(2)
  [1]=>
  int(7)
}
FLAGGED: array(3) {
  [0]=>
  int(3)
  [1]=>
  int(4)
  [2]=>
  int(5)
}
