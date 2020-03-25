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
echo "Checking with no parameters\n";
imap_open();
imap_open(false);
imap_open(false, false);
imap_open('');
imap_open('', '');

echo "Checking with incorrect parameters\n" ;
imap_open('', '', '');
imap_open('', '', '', -1);

require_once(__DIR__.'/imap_include.inc');
imap_open($default_mailbox, $username, $password, NIL, -1);

?>
--EXPECTF--
Checking with no parameters

Warning: imap_open() expects at least 3 parameters, 0 given in %s on line %d

Warning: imap_open() expects at least 3 parameters, 1 given in %s on line %d

Warning: imap_open() expects at least 3 parameters, 2 given in %s on line %d

Warning: imap_open() expects at least 3 parameters, 1 given in %s on line %d

Warning: imap_open() expects at least 3 parameters, 2 given in %s on line %d
Checking with incorrect parameters

Warning: imap_open(): Couldn't open stream  in %s on line %d

Warning: imap_open(): Couldn't open stream  in %s on line %d

Warning: imap_open(): Retries must be greater or equal to 0 in %s on line %d

Notice: Unknown: Can't open mailbox : no such mailbox (errflg=2) in Unknown on line 0
