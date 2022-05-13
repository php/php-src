--TEST--
imap_delete() passing a unique ID
--EXTENSIONS--
imap
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php

require_once(__DIR__.'/setup/imap_include.inc');

$imap_mail_box = setup_test_mailbox_for_uid_tests("imapdeleteuid", $msg_no, $uid);

imap_delete($imap_mail_box, $uid, FT_UID);
var_dump(imap_search($imap_mail_box, 'DELETED', SE_UID));
imap_expunge($imap_mail_box);

echo 'After expunging: ';
var_dump(imap_search($imap_mail_box, 'DELETED', SE_UID));

var_dump(imap_search($imap_mail_box, 'ALL', SE_UID));

imap_close($imap_mail_box);

?>
--CLEAN--
<?php
$mailbox_suffix = 'imapdeleteuid';
require_once(__DIR__ . '/setup/clean.inc');
?>
--EXPECT--
Create a temporary mailbox and add 10 msgs
New mailbox created
Delete 4 messages for Unique ID generation
array(1) {
  [0]=>
  int(9)
}
After expunging: bool(false)
array(5) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(7)
  [3]=>
  int(8)
  [4]=>
  int(10)
}
