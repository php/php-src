--TEST--
Bug GH-9309 (Segfault when connection is used after imap_close())
--EXTENSIONS--
imap
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php
require_once(__DIR__.'/setup/imap_include.inc');
$stream_id = setup_test_mailbox('gh9309', 0, $mailbox);
imap_close($stream_id);
try {
    imap_headers($stream_id);
} catch (ValueError $ex) {
    echo $ex->getMessage(), PHP_EOL;
}
?>
--CLEAN--
<?php
$mailbox_suffix = 'gh9309';
require_once(__DIR__.'/setup/clean.inc');
?>
--EXPECT--
Create a temporary mailbox and add 0 msgs
New mailbox created
IMAP\Connection is already closed
