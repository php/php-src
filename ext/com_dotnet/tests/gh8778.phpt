--TEST--
Bug GH-8778 (Integer arithmethic with large number variants fails)
--EXTENSIONS--
com_dotnet
--SKIPIF--
<?php
if (PHP_INT_SIZE < 8) die("skip for 64bit only");
?>
--FILE--
<?php
$int = 0x100000000;
var_dump(
    $int,
    new variant($int) + 1
);
?>
--EXPECT--
int(4294967296)
int(4294967297)
