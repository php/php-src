--TEST--
imap_savebody() passing a unique ID
--EXTENSIONS--
imap
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php

require_once(__DIR__.'/setup/imap_include.inc');

$imap_mail_box = setup_test_mailbox_for_uid_tests("imapsavebodyuid", $msg_no, $uid);

$section = '';

$stream_uid = fopen('php://memory', 'w+');
imap_savebody($imap_mail_box, $stream_uid, $uid, $section, FT_UID);

$stream_msg_no = fopen('php://memory', 'w+');
imap_savebody($imap_mail_box, $stream_msg_no, $msg_no, $section);

// Compare what was written.
rewind($stream_uid);
rewind($stream_msg_no);
var_dump(stream_get_contents($stream_uid) === stream_get_contents($stream_msg_no));

imap_close($imap_mail_box);

?>
--CLEAN--
<?php
$mailbox_suffix = 'imapsavebodyuid';
require_once(__DIR__ . '/setup/clean.inc');
?>
--EXPECT--
Create a temporary mailbox and add 10 msgs
New mailbox created
Delete 4 messages for Unique ID generation
bool(true)
