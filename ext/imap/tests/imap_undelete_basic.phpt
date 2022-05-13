--TEST--
imap_undelete() function : basic functionality
--CREDITS--
Olivier Doucet
--EXTENSIONS--
imap
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php

require_once(__DIR__.'/setup/imap_include.inc');
$stream_id = setup_test_mailbox('imapundeletebasic', 1);

imap_delete($stream_id, 1);

var_dump(imap_undelete($stream_id, 1));

imap_close($stream_id);

?>
--CLEAN--
<?php
$mailbox_suffix = 'imapundeletebasic';
require_once(__DIR__ . '/setup/clean.inc');
?>
--EXPECT--
Create a temporary mailbox and add 1 msgs
New mailbox created
bool(true)
