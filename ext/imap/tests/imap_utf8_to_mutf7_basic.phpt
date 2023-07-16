--TEST--
imap_utf8_to_mutf7
--EXTENSIONS--
imap
--SKIPIF--
<?php
// The underlying imap_utf8_to_mutf7 function can be missing; there's a
// ./configure check for it that disables the corresponding PHP function.
if (!function_exists('imap_utf8_to_mutf7')) {
    die("skip no imap_utf8_to_mutf7 function");
}
?>
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
