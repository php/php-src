--TEST--
imap_gc() incorrect parameter count
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
imap_gc();

echo  "Checking with incorrect parameter type\n";
imap_gc('', false);
imap_gc(false, false);

require_once(__DIR__.'/imap_include.inc');
$stream_id = imap_open($default_mailbox, $username, $password) or
	die("Cannot connect to mailbox $default_mailbox: " . imap_last_error());
imap_gc($stream_id, -1);

?>
--EXPECTF--
Checking with no parameters

Warning: imap_gc() expects exactly 2 parameters, 0 given in %s on line %d
Checking with incorrect parameter type

Warning: imap_gc() expects parameter 1 to be resource, string given in %s on line %d

Warning: imap_gc() expects parameter 1 to be resource, bool given in %s on line %d

Warning: imap_gc(): invalid value for the flags parameter in %s on line %d
