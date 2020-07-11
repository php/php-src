--TEST--
Bug #77434: php-fpm workers are segfaulting in zend_gc_addref
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

function test(int $x) {
    $a = ['a' => 0, 'b' => $x];
    $b = [];
    $b[0] = $a;
    $c = $b[0];
}

function test2(int $x) {
    $a = ['a' => 0, 'b' => $x];
    $b = [$a];
    $c = $b[0];
}

?>
===DONE===
--EXPECT--
===DONE===
