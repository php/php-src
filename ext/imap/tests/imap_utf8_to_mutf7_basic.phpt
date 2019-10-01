--TEST--
imap_utf8_to_mutf7
--SKIPIF--
<?php if (!extension_loaded("imap")) print "skip imap extension not loaded"; ?>
<?php if (!function_exists("imap_utf8_to_mutf7")) print "skip imap_utf8_to_mutf7() not available"; ?>
--FILE--
<?php

var_dump(imap_utf8_to_mutf7(""));
var_dump(imap_utf8_to_mutf7(1));
var_dump(imap_utf8_to_mutf7("täst"));

echo "Done\n";
?>
--EXPECTF--
string(0) ""
string(1) "1"
string(8) "t&AOQ-st"
Done
