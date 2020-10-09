--TEST--
imap_gc() incorrect parameter count
--CREDITS--
Paul Sohier
#phptestfest utrecht
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require_once(__DIR__.'/imap_include.inc');
$stream_id = imap_open($default_mailbox, $username, $password) or
    die("Cannot connect to mailbox $default_mailbox: " . imap_last_error());

try {
    imap_gc($stream_id, -1);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
imap_gc(): Argument #2 ($flags) must be a bitmask of IMAP_GC_TEXTS, IMAP_GC_ELT, and IMAP_GC_ENV
