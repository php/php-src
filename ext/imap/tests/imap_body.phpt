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

Warning: Wrong parameter count for imap_body() in %s on line %d
Checking with incorrect parameter type

Warning: Wrong parameter count for imap_body() in %s on line %d

Warning: Wrong parameter count for imap_body() in %s on line %d
