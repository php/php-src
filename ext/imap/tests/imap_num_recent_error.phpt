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

Warning: imap_expunge() expects exactly 1 parameter, 0 given in %s on line %d
Checking with incorrect parameter type

Warning: imap_expunge() expects parameter 1 to be resource, string given in %s on line %d

Warning: imap_expunge() expects parameter 1 to be resource, boolean given in %s on line %d
