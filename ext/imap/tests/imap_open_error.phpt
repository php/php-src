--TEST--
imap_open() incorrect parameter count
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
imap_open('', '', '', -1);

require_once(__DIR__.'/imap_include.inc');
imap_open($default_mailbox, $username, $password, NIL, -1);

?>
--EXPECTF--
Checking with incorrect parameters

Warning: imap_open(): Couldn't open stream  in %s on line %d

Warning: imap_open(): Couldn't open stream  in %s on line %d

Warning: imap_open(): Retries must be greater or equal to 0 in %s on line %d

Notice: Unknown: Can't open mailbox : no such mailbox (errflg=2) in Unknown on line 0
