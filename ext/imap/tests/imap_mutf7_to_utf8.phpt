--TEST--
imap_mutf7_to_utf8
--SKIPIF--
<?php if (!extension_loaded("imap")) print "skip"; ?>
--FILE--
<?php

var_dump(imap_mutf7_to_utf8(""));
var_dump(imap_mutf7_to_utf8(1));
var_dump(imap_mutf7_to_utf8("t&AOQ-st"));

echo "Done\n";
?>
--EXPECT--
string(0) ""
string(1) "1"
string(5) "täst"
Done
