--TEST--
imap_mutf7_to_utf8
--EXTENSIONS--
imap
--SKIPIF--
<?php
// The underlying imap_mutf7_to_utf8 function can be missing; there's a
// ./configure check for it that disables the corresponding PHP function.
if (!function_exists('imap_mutf7_to_utf8')) {
    die("skip no imap_mutf7_to_utf8 function");
}
?>
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
