--TEST--
Test imap_open() using the CL_EXPUNGE flag
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

// set up temp mailbox with 3 messages
$stream_id = setup_test_mailbox('imapopenwithclexpunge', 3, $mailbox, flags: CL_EXPUNGE);

// mark messages in inbox for deletion
for ($i = 1; $i < 4; $i++) {
    imap_delete($stream_id, $i);
}

echo "\n-- Call to imap_close() --\n";
var_dump( imap_close($stream_id) );

// check that CL_EXPUNGE in previous imap_open() call worked
$stream_id = imap_open($mailbox, IMAP_MAILBOX_USERNAME, IMAP_MAILBOX_PASSWORD);
echo "There are now " . imap_num_msg($stream_id) . " msgs in mailbox '$mailbox'\n";

// Close connection
var_dump( imap_close($stream_id) );
?>
--CLEAN--
<?php
$mailbox_suffix = 'imapopenwithclexpunge';
require_once(__DIR__.'/setup/clean.inc');
?>
--EXPECTF--
Create a temporary mailbox and add 3 msgs
New mailbox created

-- Call to imap_close() --
bool(true)
There are now 0 msgs in mailbox '%sINBOX.phpttestimapopenwithclexpunge'
bool(true)
