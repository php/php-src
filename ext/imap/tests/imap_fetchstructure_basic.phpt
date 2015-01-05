--TEST--
imap_fetchstructure() function : basic functionality 
--CREDITS--
Olivier Doucet
--SKIPIF--
<?php
require_once(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php
echo "Checking with no parameters\n";
imap_fetchstructure();

echo  "Checking with incorrect parameter type\n";
imap_fetchstructure('');
imap_fetchstructure(false);

require_once(dirname(__FILE__).'/imap_include.inc');
$stream_id = setup_test_mailbox('', 1);

imap_fetchstructure($stream_id);
imap_fetchstructure($stream_id,0);

$z = imap_fetchstructure($stream_id,1);


$fields = array('type','encoding','ifsubtype','subtype',
'ifdescription','lines','bytes','parameters');

foreach ($fields as $key) {
	var_dump(isset($z->$key));
}
var_dump($z->type);
var_dump($z->encoding);
var_dump($z->bytes);
var_dump($z->lines);
var_dump(is_object($z->parameters));

imap_close($stream_id);
?>
--CLEAN--
<?php 
require_once('clean.inc');
?>
--EXPECTF--
Checking with no parameters

Warning: imap_fetchstructure() expects at least 2 parameters, 0 given in %s on line %d
Checking with incorrect parameter type

Warning: imap_fetchstructure() expects at least 2 parameters, 1 given in %s on line %d

Warning: imap_fetchstructure() expects at least 2 parameters, 1 given in %s on line %d
Create a temporary mailbox and add 1 msgs
.. mailbox '{%s}%s' created

Warning: imap_fetchstructure() expects at least 2 parameters, 1 given in %s on line %d
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
int(%d)
int(%d)
int(%d)
int(%d)
bool(true)