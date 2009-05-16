--TEST--
imap_expunge() incorrect parameter count
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
imap_expunge();

echo  "Checking with incorrect parameter type\n";
imap_expunge('');
imap_expunge(false);
?>
--EXPECTF--
Checking with no parameters

Warning: Wrong parameter count for imap_expunge() in %s on line %d
Checking with incorrect parameter type

Warning: imap_expunge(): supplied argument is not a valid imap resource in %s on line %d

Warning: imap_expunge(): supplied argument is not a valid imap resource in %s on line %d
