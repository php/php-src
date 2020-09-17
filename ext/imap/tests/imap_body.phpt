--TEST--
imap_body() ValueError
--CREDITS--
Paul Sohier
#phptestfest utrecht
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--XFAIL--
zend_long overflow when trying to fit integer into int
--FILE--
<?php

require_once(__DIR__.'/imap_include.inc');
$stream_id = imap_open($default_mailbox, $username, $password) or
    die("Cannot connect to mailbox $default_mailbox: " . imap_last_error());

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
var_dump(imap_body($stream_id, 999, FT_UID));

imap_close($stream_id);

?>
--EXPECTF--
imap_body(): Argument #2 ($msg_no) must be greater than 0
imap_body(): Argument #3 ($options) must be a bitmask of FT_UID, FT_PEEK, and FT_INTERNAL

Warning: imap_body(): Bad message number in %s on line %d
bool(false)
