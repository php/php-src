--TEST--
Test imap_is_open()
--EXTENSIONS--
imap
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php

// include file for required variables in imap_open()
require_once(__DIR__.'/setup/imap_include.inc');

$mailbox_suffix = 'imapisopen';

// set up temp mailbox with 0 messages
$stream_id = setup_test_mailbox($mailbox_suffix, 0, $mailbox);

var_dump(imap_is_open($stream_id));

// Close connection
var_dump(imap_close($stream_id));
var_dump(imap_is_open($stream_id));

?>
--CLEAN--
<?php
$mailbox_suffix = 'imapisopen';
require_once(__DIR__.'/setup/clean.inc');
?>
--EXPECT--
Create a temporary mailbox and add 0 msgs
New mailbox created
bool(true)
bool(true)
bool(false)
