--TEST--
Test imap_mail_copy() incorrect parameters
--CREDITS--
Olivier Doucet
--SKIPIF--
<?php
require_once(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php
echo "Checking with no parameters\n";
imap_mail_copy();


echo  "Checking with incorrect parameter type\n";
imap_mail_copy('');
imap_mail_copy(false);


// more tests
require_once(dirname(__FILE__).'/imap_include.inc');


echo "Test with IMAP server\n";
$stream_id = imap_open($default_mailbox, $username, $password) or
	die("Cannot connect to mailbox $default_mailbox: " . imap_last_error());

var_dump(imap_mail_copy($stream_id));
var_dump(imap_mail_copy($stream_id,-1));
var_dump(imap_mail_copy($stream_id, ''));

imap_close($stream_id);
?>
===Done===
--EXPECTF--
Checking with no parameters

Warning: imap_mail_copy() expects at least 3 parameters, 0 given in %s on line %d
Checking with incorrect parameter type

Warning: imap_mail_copy() expects at least 3 parameters, 1 given in %s on line %d

Warning: imap_mail_copy() expects at least 3 parameters, 1 given in %s on line %d
Test with IMAP server

Warning: imap_mail_copy() expects at least 3 parameters, 1 given in %s on line %d
NULL

Warning: imap_mail_copy() expects at least 3 parameters, 2 given in %s on line %d
NULL

Warning: imap_mail_copy() expects at least 3 parameters, 2 given in %s on line %d
NULL
===Done===
