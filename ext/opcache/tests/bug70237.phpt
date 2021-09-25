--TEST--
Bug #70237 (Empty while and do-while segmentation fault with opcode on CLI enabled)
--INI--
opcache.enable=1
opcache.enable_cli=1
--EXTENSIONS--
opcache
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
