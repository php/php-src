--TEST--
Bug #70211 (php 7 ZEND_HASH_IF_FULL_DO_RESIZE use after free)
--EXTENSIONS--
soap
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php

$addr = 0x4141414141414141;

function ptr2str($ptr)
{
    $out = "";
    for ($i=0; $i<8; $i++) {
        $out .= chr($ptr & 0xff);
        $ptr >>= 8;
    }
    return $out;
}

$sf = new SoapFault('1', 'string', 'detail', 'header','line', str_repeat("A",232).ptr2str($addr));
$ob = unserialize("a:3:{i:0;".serialize($sf).'i:1;R:13;i:2;R:11;}');

var_dump($ob[1]);
?>
--EXPECT--
string(6) "detail"
