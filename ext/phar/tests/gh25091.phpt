--TEST--
GH-25091: Use-after-free in rename() when source and destination are the same entry
--EXTENSIONS--
phar
--INI--
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/gh25091.phar';

$phar = new Phar($fname);
$phar['a'] = 'contents';
unset($phar);

var_dump(rename('phar://' . $fname . '/a', 'phar://' . $fname . '/a'));
var_dump(file_get_contents('phar://' . $fname . '/a'));
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh25091.phar');
?>
--EXPECT--
bool(true)
string(8) "contents"
