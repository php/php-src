--TEST--
imap_undelete() function : basic functionality
--CREDITS--
Olivier Doucet
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require_once(__DIR__.'/imap_include.inc');
$stream_id = setup_test_mailbox('', 1);

imap_delete($stream_id, 1);

var_dump(imap_undelete($stream_id, 1));

imap_close($stream_id);

?>
--CLEAN--
<?php
require_once('clean.inc');
?>
--EXPECTF--
Create a temporary mailbox and add 1 msgs
.. mailbox '{%s}%s' created
bool(true)
