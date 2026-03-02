--TEST--
GH-7896 (Environment vars may be mangled on Windows)
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== "Windows") die("skip for Windows only");
?>
--ENV--
FÖÖ=GüИter传
--FILE--
<?php
var_dump(
    $_SERVER['FÖÖ'],
    $_ENV['FÖÖ'],
    getenv('FÖÖ')
);
?>
--EXPECT--
string(11) "GüИter传"
string(11) "GüИter传"
string(11) "GüИter传"
