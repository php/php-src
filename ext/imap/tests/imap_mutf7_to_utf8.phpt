--TEST--
imap_mutf7_to_utf8
--SKIPIF--
<?php if (!extension_loaded("imap")) print "skip"; ?>
--FILE--
<?php

var_dump(imap_mutf7_to_utf8(b""));
var_dump(imap_mutf7_to_utf8(1));
var_dump(imap_mutf7_to_utf8(array(1,2)));
var_dump(imap_mutf7_to_utf8(b"t&AOQ-st"));

echo "Done\n";
?>
--EXPECTF--	
unicode(0) ""
unicode(1) "1"

Warning: imap_mutf7_to_utf8() expects parameter 1 to be binary string, array given in %s on line %d
NULL
unicode(4) "täst"
Done
