--TEST--
Test imap_reopen() using the CL_EXPUNGE flag
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

$mailbox_suffix = 'imapreopenwithclexpunge';

// set up temp mailbox with 3 messages
$stream_id = setup_test_mailbox($mailbox_suffix , 3, $mailbox);

var_dump(imap_reopen($stream_id, IMAP_DEFAULT_MAILBOX . '.' . IMAP_MAILBOX_PHPT_PREFIX . $mailbox_suffix, flags: CL_EXPUNGE));

// mark messages in inbox for deletion
for ($i = 1; $i < 4; $i++) {
    imap_delete($stream_id, $i);
}

echo "\n-- Call to imap_close() --\n";
var_dump( imap_close($stream_id) );

// check that CL_EXPUNGE in previous imap_reopen() call worked
$stream_id = imap_open($mailbox, IMAP_MAILBOX_USERNAME, IMAP_MAILBOX_PASSWORD);
echo "There are now " . imap_num_msg($stream_id) . " msgs in mailbox '$mailbox'\n";

// Close connection
var_dump( imap_close($stream_id) );
?>
--CLEAN--
<?php
$mailbox_suffix = 'imapreopenwithclexpunge';
require_once(__DIR__.'/setup/clean.inc');
?>
--EXPECTF--
Create a temporary mailbox and add 3 msgs
New mailbox created
bool(true)

-- Call to imap_close() --
bool(true)
There are now 0 msgs in mailbox '%sINBOX.phpttestimapreopenwithclexpunge'
bool(true)
