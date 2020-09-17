--TEST--
imap_open() ValueErrors
--CREDITS--
Paul Sohier
#phptestfest utrecht
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

echo "Checking with incorrect parameters\n" ;
imap_open('', '', '');

try {
    imap_open('', '', '', -1);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

require_once(__DIR__.'/imap_include.inc');

try {
    imap_open($default_mailbox, $username, $password, NIL, -1);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECTF--
Checking with incorrect parameters

Warning: imap_open(): Couldn't open stream  in %s on line %d
imap_open(): Argument #4 ($options) must be a bitmask of OP_READONLY, OP_ANONYMOUS, OP_HALFOPEN, CL_EXPUNGE, OP_DEBUG, OP_SHORTCACHE, OP_SILENT, OP_PROTOTYPE, and OP_SECURE
imap_open(): Argument #5 ($n_retries) must be greater than or equal to 0

Notice: Unknown: Can't open mailbox : no such mailbox (errflg=2) in Unknown on line 0
