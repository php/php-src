--TEST--
Bug #77454: mb_scrub() silently truncates after a null byte
--SKIPIF--
<?php require 'skipif.inc'; ?>
--FILE--
<?php
$str = "before\0after";
function test($str, $enc) {
    echo str_replace("\0", '\0', mb_scrub($str, $enc)), "\n";
}
test($str, 'latin1');
test($str, 'utf-8');
test($str, 'ascii');
?>
--EXPECT--
before\0after
before\0after
before\0after
