--TEST--
Test imap_close() function : basic functionality
--EXTENSIONS--
imap
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php
echo "*** Testing imap_close() : basic functionality ***\n";

// include file for required variables in imap_open()
require_once(__DIR__.'/setup/imap_include.inc');

// Initialize required variables
$stream_id = setup_test_mailbox('imapclosebasic', 3, $mailbox); // set up temp mailbox with 3 messages
$options = CL_EXPUNGE;

// mark messages in inbox for deletion
for ($i = 1; $i < 4; $i++) {
    imap_delete($stream_id, $i);
}

// Calling imap_close() with all possible arguments
echo "\n-- Call to imap_close() with all possible arguments --\n";
var_dump( imap_close($stream_id, $options) );

// check that CL_EXPUNGE worked
$stream_id = imap_open($mailbox, IMAP_MAILBOX_USERNAME, IMAP_MAILBOX_PASSWORD);
echo "There are now " . imap_num_msg($stream_id) . " msgs in mailbox '$mailbox'\n";

// Calling imap_close() with mandatory arguments
echo "\n-- Call to imap_close() with mandatory arguments --\n";
var_dump( imap_close($stream_id) );
?>
--CLEAN--
<?php
$mailbox_suffix = 'imapclosebasic';
require_once(__DIR__.'/setup/clean.inc');
?>
--EXPECTF--
*** Testing imap_close() : basic functionality ***
Create a temporary mailbox and add 3 msgs
New mailbox created

-- Call to imap_close() with all possible arguments --
bool(true)
There are now 0 msgs in mailbox '%sINBOX.phpttestimapclosebasic'

-- Call to imap_close() with mandatory arguments --
bool(true)
