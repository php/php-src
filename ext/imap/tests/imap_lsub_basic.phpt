--TEST--
imap_lsub() function : basic functionality
--CREDITS--
Olivier Doucet
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
if (getenv("SKIP_ASAN")) die("skip leak sanitizer crashes");
?>
--CONFLICTS--
defaultmailbox
--FILE--
<?php

require_once(__DIR__.'/setup/imap_include.inc');
$stream_id = imap_open(IMAP_DEFAULT_MAILBOX, IMAP_MAILBOX_USERNAME, IMAP_MAILBOX_PASSWORD) or
    die("Cannot connect to mailbox " .IMAP_DEFAULT_MAILBOX. ": " . imap_last_error());

var_dump(imap_lsub($stream_id, IMAP_DEFAULT_MAILBOX, 'ezDvfXvbvcxSerz'));


echo "Checking OK\n";

$newbox = IMAP_DEFAULT_MAILBOX . "." . IMAP_MAILBOX_PHPT_PREFIX;

imap_createmailbox($stream_id, $newbox);
imap_subscribe($stream_id, $newbox);

$z = imap_lsub($stream_id, IMAP_DEFAULT_MAILBOX, '*');

var_dump(is_array($z));

// e.g. "{127.0.0.1:143/norsh}INBOX.phpttest"
var_dump($z[0]);

imap_close($stream_id);
?>
--CLEAN--
<?php
$mailbox_suffix = '';
require_once(__DIR__.'/setup/clean.inc');
?>
--EXPECTF--
bool(false)
Checking OK
bool(true)
string(%s) "{%s}%s"
