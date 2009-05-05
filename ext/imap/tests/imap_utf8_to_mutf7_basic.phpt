--TEST--
imap_utf8_to_mutf7
--SKIPIF--
<?php if (!extension_loaded("imap")) print "skip"; ?>
<?php if (!function_exists("imap_utf8_to_mutf7")) print "skip"; ?>
--FILE--
<?php

var_dump(imap_utf8_to_mutf7(""));
var_dump(imap_utf8_to_mutf7(1));
var_dump(imap_utf8_to_mutf7(array(1,2)));
var_dump(imap_utf8_to_mutf7("täst"));

echo "Done\n";
?>
--EXPECTF--	
string(0) ""
string(1) "1"

Warning: imap_utf8_to_mutf7() expects parameter 1 to be string, array given in %s on line %d
NULL
string(8) "t&AOQ-st"
Done
