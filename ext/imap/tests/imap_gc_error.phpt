--TEST--
imap_gc() ValueError
--CREDITS--
Paul Sohier
#phptestfest utrecht
--EXTENSIONS--
imap
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php

require_once(__DIR__.'/setup/imap_include.inc');
$stream_id = setup_test_mailbox('imapgcerror', 1);

try {
    imap_gc($stream_id, -1);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--CLEAN--
<?php
$mailbox_suffix = 'imapgcerror';
require_once(__DIR__.'/setup/clean.inc');
?>
--EXPECT--
Create a temporary mailbox and add 1 msgs
New mailbox created
imap_gc(): Argument #2 ($flags) must be a bitmask of IMAP_GC_TEXTS, IMAP_GC_ELT, and IMAP_GC_ENV
