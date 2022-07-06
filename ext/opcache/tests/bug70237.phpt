--TEST--
Bug #70237 (Empty while and do-while segmentation fault with opcode on CLI enabled)
--INI--
opcache.enable=1
opcache.enable_cli=1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php


function neverUsed() {
    $bool = false;
    while ($bool) { };
    //do { } while ($bool);
}
?>
okey
--EXPECT--
okey
