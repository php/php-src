--TEST--
Test imap_fetch_overview() function: invalid option
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php

require_once __DIR__.'/setup/imap_include.inc';

// Initialise required variables
$stream_id = setup_test_mailbox('imapfetchoverviewvar3', 1); // set up temporary mailbox with one simple message
$msg_no = 1;
$msg_uid = imap_uid($stream_id, $msg_no);
imap_check($stream_id);

try {
    imap_fetch_overview($stream_id, $msg_uid, 245);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--CLEAN--
<?php
$mailbox_suffix = 'imapfetchoverviewvar3';
require_once(__DIR__.'/setup/clean.inc');
?>
--EXPECT--
imap_fetch_overview(): Argument #3 ($flags) must be FT_UID or 0
