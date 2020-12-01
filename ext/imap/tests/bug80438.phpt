--TEST--
Bug #80438: imap_msgno() incorrectly warns and return false on valid UIDs in PHP 8.0.0
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php

require_once __DIR__.'/setup/imap_include.inc';

// create a new mailbox and add 10 new messages to it
$mail_box = setup_test_mailbox('bug80438', 10);

// Delete messages to remove the numerical ordering
imap_delete($mail_box, 5);
imap_delete($mail_box, 6);
imap_delete($mail_box, 7);
imap_delete($mail_box, 8);
imap_expunge($mail_box);

$message_number_array = imap_search($mail_box, 'ALL', SE_UID);

var_dump($message_number_array);

foreach ($message_number_array as $message_unique_id)
{
    echo 'Unique ID: ';
    var_dump($message_unique_id);
    echo 'Ordered message number: ';
    var_dump(imap_msgno($mail_box, $message_unique_id));
}

imap_close($mail_box);

?>
--CLEAN--
<?php
$mailbox_suffix = 'bug80438';
require_once __DIR__.'/setup/clean.inc';
?>
--EXPECT--
Create a temporary mailbox and add 10 msgs
New mailbox created
array(6) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(9)
  [5]=>
  int(10)
}
Unique ID: int(1)
Ordered message number: int(1)
Unique ID: int(2)
Ordered message number: int(2)
Unique ID: int(3)
Ordered message number: int(3)
Unique ID: int(4)
Ordered message number: int(4)
Unique ID: int(9)
Ordered message number: int(5)
Unique ID: int(10)
Ordered message number: int(6)
