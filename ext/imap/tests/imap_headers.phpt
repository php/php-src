--TEST--
imap_headers() incorrect parameter count
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
imap_headers();

echo  "Checking with incorrect parameter type\n";
imap_headers('');
imap_headers(false);
?>
--EXPECTF--
Checking with no parameters

Warning: Wrong parameter count for imap_headers() in %s on line %d
Checking with incorrect parameter type

Warning: imap_headers(): supplied argument is not a valid imap resource in %s on line %d

Warning: imap_headers(): supplied argument is not a valid imap resource in %s on line %d
