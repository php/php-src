--TEST--
GH-21770 (Infinite recursion in property hook getter in opcache preloaded trait)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_gh21770.inc
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
$b = new B();
echo $b->a, "\n";

$c = new C();
$c->x = 42;
var_dump($c->x);
?>
--EXPECT--
a
int(42)
