--TEST--
imap_body() ValueError
--CREDITS--
Paul Sohier
#phptestfest utrecht
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php

require_once(__DIR__.'/setup/imap_include.inc');

$imap_stream = setup_test_mailbox("imapbodyvalueerror", 0);

try {
    imap_body($imap_stream,-1);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    imap_body($imap_stream,1,-1);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

//Access not existing
var_dump(imap_body($imap_stream, 255, FT_UID));

imap_close($imap_stream);

?>
--CLEAN--
<?php
$mailbox_suffix = 'imapbodyvalueerror';
require_once(__DIR__ . '/setup/clean.inc');
?>
--EXPECTF--
Create a temporary mailbox and add 0 msgs
New mailbox created
imap_body(): Argument #2 ($message_num) must be greater than 0
imap_body(): Argument #3 ($flags) must be a bitmask of FT_UID, FT_PEEK, and FT_INTERNAL

Warning: imap_body(): UID does not exist in %s on line %d
bool(false)
