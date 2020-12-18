--TEST--
imap_body() errors: ValueError and Warnings
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

$imap_mail_box = setup_test_mailbox("imapbodyerror", 0);

try {
    imap_body($imap_mail_box, -1);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    imap_body($imap_mail_box, 1, -1);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

// Access not existing
var_dump(imap_body($imap_mail_box, 255));
var_dump(imap_body($imap_mail_box, 255, FT_UID));

imap_close($imap_mail_box);

?>
--CLEAN--
<?php
$mailbox_suffix = 'imapbodyerror';
require_once(__DIR__ . '/setup/clean.inc');
?>
--EXPECTF--
Create a temporary mailbox and add 0 msgs
New mailbox created
imap_body(): Argument #2 ($message_num) must be greater than 0
imap_body(): Argument #3 ($flags) must be a bitmask of FT_UID, FT_PEEK, and FT_INTERNAL

Warning: imap_body(): Bad message number in %s on line %d
bool(false)

Warning: imap_body(): UID does not exist in %s on line %d
bool(false)
