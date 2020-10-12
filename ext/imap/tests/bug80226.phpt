--TEST--
Bug #80226 (imap_sort() leaks sortpgm memory)
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php
require_once(__DIR__.'/imap_include.inc');
$stream = imap_open($default_mailbox, $username, $password);
imap_sort($stream, SORTFROM, 0);
?>
--EXPECT--
