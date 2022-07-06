--TEST--
imap_sort() basics
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php

require_once(__DIR__.'/setup/imap_include.inc');

$imap_mail_box = setup_test_mailbox_for_uid_tests("imapsortbasic");

var_dump(imap_sort($imap_mail_box, SORTSUBJECT, 0));
var_dump(imap_sort($imap_mail_box, SORTSUBJECT, 0, SE_UID));

imap_close($imap_mail_box);

?>
--CLEAN--
<?php
$mailbox_suffix = 'imapsortbasic';
require_once(__DIR__ . '/setup/clean.inc');
?>
--EXPECT--
Create a temporary mailbox and add 10 msgs
New mailbox created
Delete 4 messages for Unique ID generation
array(6) {
  [0]=>
  int(1)
  [1]=>
  int(6)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(4)
  [5]=>
  int(5)
}
array(6) {
  [0]=>
  int(1)
  [1]=>
  int(10)
  [2]=>
  int(2)
  [3]=>
  int(7)
  [4]=>
  int(8)
  [5]=>
  int(9)
}
