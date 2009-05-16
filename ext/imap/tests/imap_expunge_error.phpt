--TEST--
imap_num_recent() incorrect parameter count
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
imap_num_recent();

echo  "Checking with incorrect parameter type\n";
imap_num_recent('');
imap_num_recent(false);
?>
--EXPECTF--
Checking with no parameters

Warning: Wrong parameter count for imap_num_recent() in %s on line %d
Checking with incorrect parameter type

Warning: imap_num_recent(): supplied argument is not a valid imap resource in %s on line %d

Warning: imap_num_recent(): supplied argument is not a valid imap resource in %s on line %d
