--TEST--
Assignment through a float typed reference must not reuse the assigned integer range
--EXTENSIONS--
opcache
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
class Box {
    public float $real = 0.0;
}

function assignGlobalFloat() {
    return ($GLOBALS['real'] = 9007199254740993) & 1;
}

$box = new Box();
$GLOBALS['real'] = &$box->real;
var_dump(assignGlobalFloat());
?>
--EXPECT--
int(0)
