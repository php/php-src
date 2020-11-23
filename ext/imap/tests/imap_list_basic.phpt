--TEST--
imap_list() function : basic functionality
--CREDITS--
Olivier Doucet
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php

require_once(__DIR__.'/setup/imap_include.inc');
$stream_id = imap_open(IMAP_DEFAULT_MAILBOX, IMAP_MAILBOX_USERNAME, IMAP_MAILBOX_PASSWORD) or
    die("Cannot connect to mailbox " .IMAP_DEFAULT_MAILBOX.": " . imap_last_error());

imap_list($stream_id, IMAP_DEFAULT_MAILBOX,'ezerz');


$z = imap_list($stream_id, IMAP_DEFAULT_MAILBOX,'*');
var_dump(is_array($z));

// e.g. "{127.0.0.1:143/norsh}INBOX"
var_dump($z[0]);

imap_close($stream_id);
?>
--EXPECTF--
bool(true)
string(%s) "{%s}%s"
