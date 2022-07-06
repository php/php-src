--TEST--
imap_utf8() tests
--SKIPIF--
<?php if (!extension_loaded("imap")) print "skip"; ?>
--FILE--
<?php

var_dump(imap_utf8(""));
var_dump(imap_utf8(1));
var_dump(imap_utf8("test"));

echo "Done\n";
?>
--EXPECT--
string(0) ""
string(1) "1"
string(4) "test"
Done
