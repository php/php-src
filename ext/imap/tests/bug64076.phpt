--TEST--
Bug #64076 (imap_sort() does not return FALSE on failure)
--SKIPIF--
<?php
require_once __DIR__ . '/setup/skipif.inc';
?>
--FILE--
<?php
require_once __DIR__ . '/setup/imap_include.inc';
$stream = setup_test_mailbox('bug64076', 2);
imap_errors(); // clear error stack
var_dump(imap_sort($stream, SORTFROM, 0, 0, 'UNSUPPORTED SEARCH CRITERIUM'));
var_dump(imap_errors() !== false);
?>
--CLEAN--
<?php
$mailbox_suffix = 'bug64076';
require_once(__DIR__ . '/setup/clean.inc');
?>
--EXPECT--
Create a temporary mailbox and add 2 msgs
New mailbox created
bool(false)
bool(true)
