--TEST--
imap_getsubscribed() function : basic functionality
--CREDITS--
Olivier Doucet
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php
echo "Checking with no parameters\n";
imap_getsubscribed();

echo  "Checking with incorrect parameter type\n";
imap_getsubscribed('');
imap_getsubscribed(false);

require_once(__DIR__.'/imap_include.inc');
$stream_id = imap_open($default_mailbox, $username, $password) or
	die("Cannot connect to mailbox $default_mailbox: " . imap_last_error());

imap_getsubscribed($stream_id);
imap_getsubscribed($stream_id,$default_mailbox);
var_dump(imap_getsubscribed($stream_id,$default_mailbox,'ezDvfXvbvcxSerz'));


echo "Checking OK\n";

$newbox = $default_mailbox . "." . $mailbox_prefix;

imap_createmailbox($stream_id, $newbox);
imap_subscribe($stream_id, $newbox);

$z = imap_getsubscribed($stream_id,$default_mailbox,'*');

var_dump(is_array($z));
var_dump($z[0]);

imap_close($stream_id);
?>
--CLEAN--
<?php
require_once('clean.inc');
?>
--EXPECTF--
Checking with no parameters

Warning: imap_getsubscribed() expects exactly 3 parameters, 0 given in %s on line %d
Checking with incorrect parameter type

Warning: imap_getsubscribed() expects exactly 3 parameters, 1 given in %s on line %d

Warning: imap_getsubscribed() expects exactly 3 parameters, 1 given in %s on line %d

Warning: imap_getsubscribed() expects exactly 3 parameters, 1 given in %s on line %d

Warning: imap_getsubscribed() expects exactly 3 parameters, 2 given in %s on line %d
bool(false)
Checking OK
bool(true)
object(stdClass)#%d (%d) {
  [%sname"]=>
  string(%d) "{%s}%s"
  [%sattributes"]=>
  int(%d)
  [%sdelimiter"]=>
  string(%d) "%s"
}
