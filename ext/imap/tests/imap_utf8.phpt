--TEST--
imap_utf8() tests
--SKIPIF--
<?php if (!extension_loaded("imap")) print "skip"; ?>
--FILE--
<?php

var_dump(imap_utf8(""));
var_dump(imap_utf8(1));
var_dump(imap_utf8(array(1,2)));
var_dump(imap_utf8("test"));

echo "Done\n";
?>
--EXPECTF--
string(0) ""
string(1) "1"

Warning: imap_utf8() expects parameter 1 to be string, array given in %s on line %d
NULL
string(4) "%s"
Done
