--TEST--
Bug #80438: imap_msgno() incorrectly warns and return false on valid UIDs in PHP 8.0.0
--EXTENSIONS--
imap
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php

require_once __DIR__.'/setup/imap_include.inc';

// create a new mailbox and add 10 new messages to it
$mail_box = setup_test_mailbox_for_uid_tests('bug80438');

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
Unique ID: int(1)
Ordered message number: int(1)
Unique ID: int(2)
Ordered message number: int(2)
Unique ID: int(7)
Ordered message number: int(3)
Unique ID: int(8)
Ordered message number: int(4)
Unique ID: int(9)
Ordered message number: int(5)
Unique ID: int(10)
Ordered message number: int(6)
