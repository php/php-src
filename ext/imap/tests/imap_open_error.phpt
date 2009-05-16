--TEST--
imap_open() incorrect parameter count
--CREDITS--
Paul Sohier
#phptestfest utrecht
--SKIPIF--
<?php
require_once(dirname(__FILE__).'/skipif.inc');
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

?>
--EXPECTF--
Checking with no parameters

Warning: Wrong parameter count for imap_open() in %s on line %d

Warning: Wrong parameter count for imap_open() in %s on line %d

Warning: Wrong parameter count for imap_open() in %s on line %d

Warning: Wrong parameter count for imap_open() in %s on line %d

Warning: Wrong parameter count for imap_open() in %s on line %d
Checking with incorrect parameters

Warning: imap_open(): Couldn't open stream  in %s on line %d

Warning: imap_open(): Couldn't open stream  in %s on line %d

Notice: Unknown: Can't open mailbox : no such mailbox (errflg=2) in Unknown on line 0
