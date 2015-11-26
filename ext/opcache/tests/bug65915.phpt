--TEST--
Bug #65915 (Inconsistent results with require return value)
--INI--
opcache.enable=1
opcache.enable_cli=1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$tmp = __DIR__ . "/bug65915.inc.php";

file_put_contents($tmp, '<?php return function(){ return "a";};');
$f = require $tmp;
var_dump($f());

opcache_invalidate($tmp, true);

file_put_contents($tmp, '<?php return function(){ return "b";};');
$f = require $tmp;
var_dump($f());

@unlink($tmp);
?>
--EXPECT--
string(1) "a"
string(1) "b"
