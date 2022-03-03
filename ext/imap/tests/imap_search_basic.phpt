--TEST--
imap_search() with unique ID (SE_UID) flag
--EXTENSIONS--
imap
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php

require_once(__DIR__.'/setup/imap_include.inc');

$imap_mail_box = setup_test_mailbox_for_uid_tests("imapsearchuid");

var_dump(imap_search($imap_mail_box, 'ALL', SE_UID));

imap_close($imap_mail_box);

?>
--CLEAN--
<?php
$mailbox_suffix = 'imapsearchuid';
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
