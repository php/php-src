--TEST--
imap_renamemailbox() function : basic functionality
--CREDITS--
Olivier Doucet
--SKIPIF--
<?php
require_once(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php
echo "Checking with no parameters\n";
imap_renamemailbox();

echo  "Checking with incorrect parameter type\n";
imap_renamemailbox('');
imap_renamemailbox(false);


require_once(dirname(__FILE__).'/imap_include.inc');

$stream_id = setup_test_mailbox('', 1);

if (!is_resource($stream_id)) {
	exit("TEST FAILED: Unable to create test mailbox\n");
}

$newbox = $default_mailbox . "." . $mailbox_prefix;

imap_renamemailbox($stream_id, $newbox.'not');
imap_renamemailbox($stream_id, $newbox);

//commented because of bug #49901
//$ancError = error_reporting(0);
//$z = imap_renamemailbox($stream_id, $newbox.'not2', $newbox.'2');
//var_dump($z);
//error_reporting($ancError);
echo "Checking OK\n";


var_dump(imap_createmailbox($stream_id, $newbox.'.test'));
var_dump(imap_renamemailbox($stream_id, $newbox.'.test', $newbox.'.testd'));

imap_close($stream_id);
?>
--CLEAN--
<?php
require_once('clean.inc');
?>
--EXPECTF--
Checking with no parameters

Warning: imap_renamemailbox() expects exactly 3 parameters, 0 given in %s on line %d
Checking with incorrect parameter type

Warning: imap_renamemailbox() expects exactly 3 parameters, 1 given in %s on line %d

Warning: imap_renamemailbox() expects exactly 3 parameters, 1 given in %s on line %d
Create a temporary mailbox and add 1 msgs
.. mailbox '{%s}%s' created

Warning: imap_renamemailbox() expects exactly 3 parameters, 2 given in %s on line %d

Warning: imap_renamemailbox() expects exactly 3 parameters, 2 given in %s on line %d
Checking OK
bool(true)
bool(true)
