--TEST--
GH-25090 (Use-after-free in phar_wrapper_rename() with entry metadata)
--EXTENSIONS--
phar
--INI--
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/gh25090.phar';
$phar = new Phar($fname);
$phar['a'] = 'x';
$phar['a']->setMetadata(['k' => 'v']);
rename('phar://' . $fname . '/a', 'phar://' . $fname . '/b');
$phar['c'] = 'y';
var_dump($phar['b']->getMetadata());
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh25090.phar');
?>
--EXPECT--
array(1) {
  ["k"]=>
  string(1) "v"
}
