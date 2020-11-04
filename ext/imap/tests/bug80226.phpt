--TEST--
Bug #80226 (imap_sort() leaks sortpgm memory)
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php
require_once(__DIR__.'/setup/imap_include.inc');

echo "Create a new mailbox for test\n";

$stream = setup_test_mailbox("bug80226", 0);
var_dump(imap_sort($stream, SORTFROM, 0));
?>
--CLEAN--
<?php
$mailbox_suffix = 'bug80226';
require_once(__DIR__ . '/setup/clean.inc');
?>
--EXPECT--
Create a new mailbox for test
Create a temporary mailbox and add 0 msgs
New mailbox created
array(0) {
}
