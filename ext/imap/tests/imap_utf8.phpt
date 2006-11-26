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

Notice: Array to string conversion in %s on line %d
string(5) "%s"
string(4) "%s"
Done
