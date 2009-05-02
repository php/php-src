--TEST--
imap_utf8_to_mutf7
--SKIPIF--
<?php if (!extension_loaded("imap")) print "skip"; ?>
--FILE--
<?php

var_dump(imap_utf8_to_mutf7(""));
var_dump(imap_utf8_to_mutf7(1));
var_dump(imap_utf8_to_mutf7(array(1,2)));

$test = 't' . chr(228) . 'st';
var_dump(imap_utf8_to_mutf7(utf8_encode($test)));

echo "Done\n";
?>
--EXPECTF--	
string(0) ""
string(1) "1"

Warning: imap_utf8_to_mutf7() expects parameter 1 to be binary string, array given in %s on line %d
NULL
string(8) "t&AOQ-st"
Done
