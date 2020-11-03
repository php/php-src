--TEST--
Bug #80226 (imap_sort() leaks sortpgm memory)
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--CONFLICTS--
defaultmailbox
--FILE--
<?php
require_once(__DIR__.'/setup/imap_include.inc');
$stream = imap_open(IMAP_DEFAULT_MAILBOX, IMAP_MAILBOX_USERNAME, IMAP_MAILBOX_PASSWORD);
var_dump(imap_sort($stream, SORTFROM, 0));
?>
--EXPECT--
array(0) {
}
