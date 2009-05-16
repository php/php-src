--TEST--
imap_body() incorrect parameter count
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
imap_body();

echo  "Checking with incorrect parameter type\n";
imap_body('');
imap_body(false);
?>
--EXPECTF--
Checking with no parameters

Warning: imap_body() expects at least 2 parameters, 0 given in %s on line %d
Checking with incorrect parameter type

Warning: imap_body() expects at least 2 parameters, 1 given in %s on line %d

Warning: imap_body() expects at least 2 parameters, 1 given in %s on line %d
