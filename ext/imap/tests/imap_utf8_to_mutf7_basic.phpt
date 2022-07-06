--TEST--
imap_utf8_to_mutf7
--SKIPIF--
<?php if (!extension_loaded("imap")) print "skip imap extension not loaded"; ?>
--FILE--
<?php

var_dump(imap_utf8_to_mutf7(""));
var_dump(imap_utf8_to_mutf7(1));
var_dump(imap_utf8_to_mutf7("täst"));

echo "Done\n";
?>
--EXPECT--
string(0) ""
string(1) "1"
string(8) "t&AOQ-st"
Done
