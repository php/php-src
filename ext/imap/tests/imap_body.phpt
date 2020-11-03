--TEST--
imap_body() ValueError
--CREDITS--
Paul Sohier
#phptestfest utrecht
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--CONFLICTS--
# Might not be needed
defaultmailbox
--FILE--
<?php

require_once(__DIR__.'/setup/imap_include.inc');
$stream_id = imap_open(IMAP_SERVER, IMAP_MAILBOX_USERNAME, IMAP_MAILBOX_PASSWORD) or
    die("Cannot connect to mailbox ". IMAP_SERVER. ": " . imap_last_error());

try {
    imap_body($stream_id,-1);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    imap_body($stream_id,1,-1);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

//Access not existing
var_dump(imap_body($stream_id, 255, FT_UID));

imap_close($stream_id);

?>
--EXPECTF--
imap_body(): Argument #2 ($message_num) must be greater than 0
imap_body(): Argument #3 ($flags) must be a bitmask of FT_UID, FT_PEEK, and FT_INTERNAL

Warning: imap_body(): UID does not exist in %s on line %d
bool(false)
