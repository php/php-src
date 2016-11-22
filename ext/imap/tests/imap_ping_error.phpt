--TEST--
imap_ping() incorrect parameter count
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
imap_ping();

echo  "Checking with incorrect parameter type\n";
imap_ping('');
imap_ping(false);
?>
--EXPECTF--
Checking with no parameters

Warning: imap_ping() expects exactly 1 parameter, 0 given in %s on line %d
Checking with incorrect parameter type

Warning: imap_ping() expects parameter 1 to be resource, string given in %s on line %d

Warning: imap_ping() expects parameter 1 to be resource, boolean given in %s on line %d
