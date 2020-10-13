--TEST--
Bug #64076 (imap_sort() does not return FALSE on failure)
--SKIPIF--
<?php
require_once __DIR__ . '/skipif.inc';
?>
--FILE--
<?php
require_once __DIR__ . '/imap_include.inc';
$stream = setup_test_mailbox('', 2);
imap_errors(); // clear error stack
var_dump(imap_sort($stream, SORTFROM, 0, 0, 'UNSUPPORTED SEARCH CRITERIUM'));
var_dump(imap_errors() !== false);
?>
--CLEAN--
<?php
require_once __DIR__ . '/clean.inc';
?>
--EXPECT--
Create a temporary mailbox and add 2 msgs
.. mailbox '{127.0.0.1:143/norsh}INBOX.phpttest' created
bool(false)
bool(true)
