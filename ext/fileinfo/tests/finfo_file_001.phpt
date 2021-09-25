--TEST--
finfo_file(): Testing file names
--EXTENSIONS--
fileinfo
--FILE--
<?php

$fp = finfo_open();
try {
    var_dump(finfo_file($fp, "\0"));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(finfo_file($fp, ''));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump(finfo_file($fp, '.'));
var_dump(finfo_file($fp, '&'));

?>
--EXPECTF--
finfo_file(): Argument #1 ($finfo) must not contain any null bytes
finfo_file(): Argument #1 ($finfo) cannot be empty
string(9) "directory"

Warning: finfo_file(&): Failed to open stream: No such file or directory in %s on line %d
bool(false)
