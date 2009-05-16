--TEST--
imap_num_msg() incorrect parameter count
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
imap_num_msg();

echo  "Checking with incorrect parameter type\n";
imap_num_msg('');
imap_num_msg(false);
?>
--EXPECTF--
Checking with no parameters

Warning: Wrong parameter count for imap_num_msg() in %s on line %d
Checking with incorrect parameter type

Warning: imap_num_msg(): supplied argument is not a valid imap resource in %s on line %d

Warning: imap_num_msg(): supplied argument is not a valid imap resource in %s on line %d
