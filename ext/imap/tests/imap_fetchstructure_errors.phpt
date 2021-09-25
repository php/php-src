--TEST--
imap_fetchstructure() errors: ValueError and Warnings
--EXTENSIONS--
imap
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php

require_once(__DIR__.'/setup/imap_include.inc');

$imap_mail_box = setup_test_mailbox("imapfetchstructureerrors", 0);

try {
    imap_fetchstructure($imap_mail_box, -1);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    imap_fetchstructure($imap_mail_box, 1, -1);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

// Access not existing
var_dump(imap_fetchstructure($imap_mail_box, 255));
var_dump(imap_fetchstructure($imap_mail_box, 255, FT_UID));

imap_close($imap_mail_box);

?>
--CLEAN--
<?php
$mailbox_suffix = 'imapfetchstructureerrors';
require_once(__DIR__ . '/setup/clean.inc');
?>
--EXPECTF--
Create a temporary mailbox and add 0 msgs
New mailbox created
imap_fetchstructure(): Argument #2 ($message_num) must be greater than 0
imap_fetchstructure(): Argument #3 ($flags) must be FT_UID or 0

Warning: imap_fetchstructure(): Bad message number in %s on line %d
bool(false)

Warning: imap_fetchstructure(): UID does not exist in %s on line %d
bool(false)
