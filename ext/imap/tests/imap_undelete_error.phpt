--TEST--
imap_undelete() incorrect parameter count
--CREDITS--
Olivier Doucet
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php
echo "Checking with no parameters\n";
imap_undelete();

echo  "Checking with incorrect parameter type\n";
imap_undelete('');
imap_undelete(false);

require_once(__DIR__.'/imap_include.inc');
$stream_id = imap_open($default_mailbox, $username, $password) or
	die("Cannot connect to mailbox $default_mailbox: " . imap_last_error());

imap_undelete($stream_id);

imap_close($stream_id);
?>
--EXPECTF--
Checking with no parameters

Warning: imap_undelete() expects at least 2 parameters, 0 given in %s on line %d
Checking with incorrect parameter type

Warning: imap_undelete() expects at least 2 parameters, 1 given in %s on line %d

Warning: imap_undelete() expects at least 2 parameters, 1 given in %s on line %d

Warning: imap_undelete() expects at least 2 parameters, 1 given in %s on line %d
