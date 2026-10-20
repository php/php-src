--TEST--
phpcredits() full-page HTML title
--POST--
dummy=x
--FILE--
<?php
ob_start();
$result = phpcredits();
$output = ob_get_clean();

var_dump($result);
var_dump(str_contains($output, '<title>PHP ' . PHP_VERSION . ' - phpcredits()</title>'));
var_dump(str_contains($output, '<title>PHP ' . PHP_VERSION . ' - phpinfo()</title>'));
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
