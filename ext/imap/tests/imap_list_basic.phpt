--TEST--
imap_list() function : basic functionality 
--CREDITS--
Olivier Doucet
--SKIPIF--
<?php
require_once(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php
echo "Checking with no parameters\n";
imap_list();

echo  "Checking with incorrect parameter type\n";
imap_list('');
imap_list(false);

require_once(dirname(__FILE__).'/imap_include.inc');
$stream_id = imap_open($default_mailbox, $username, $password) or 
	die("Cannot connect to mailbox $default_mailbox: " . imap_last_error());

imap_list($stream_id);
imap_list($stream_id,$default_mailbox);
imap_list($stream_id,$default_mailbox,'ezerz');


$z = imap_list($stream_id,$default_mailbox,'*');
var_dump(is_array($z));
var_dump($z[0]);

imap_close($stream_id);
?>
--EXPECTF--
Checking with no parameters

Warning: imap_list() expects exactly 3 parameters, 0 given in %s on line %d
Checking with incorrect parameter type

Warning: imap_list() expects exactly 3 parameters, 1 given in %s on line %d

Warning: imap_list() expects exactly 3 parameters, 1 given in %s on line %d

Warning: imap_list() expects exactly 3 parameters, 1 given in %s on line %d

Warning: imap_list() expects exactly 3 parameters, 2 given in %s on line %d
bool(true)
string(%s) "{%s}%s"