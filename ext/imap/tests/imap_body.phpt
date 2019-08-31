--TEST--
imap_body() incorrect parameter count
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
imap_body();

echo  "Checking with incorrect parameter type\n";
imap_body('');
imap_body(false);
require_once(__DIR__.'/imap_include.inc');
$stream_id = imap_open($default_mailbox, $username, $password) or
	die("Cannot connect to mailbox $default_mailbox: " . imap_last_error());
imap_body($stream_id);
imap_body($stream_id,-1);
imap_body($stream_id,1,-1);

//Access not existing
var_dump(imap_body($stream_id, 999, FT_UID));

imap_close($stream_id);

?>
--EXPECTF--
Checking with no parameters

Warning: imap_body() expects at least 2 parameters, 0 given in %s on line %d
Checking with incorrect parameter type

Warning: imap_body() expects at least 2 parameters, 1 given in %s on line %d

Warning: imap_body() expects at least 2 parameters, 1 given in %s on line %d

Warning: imap_body() expects at least 2 parameters, 1 given in %s on line %d

Warning: imap_body(): Bad message number in %s on line %d

Warning: imap_body(): invalid value for the options parameter in %s on line %d

Warning: imap_body(): Bad message number in %s on line %d
bool(false)
