--TEST--
Bug #70211 (php 7 ZEND_HASH_IF_FULL_DO_RESIZE use after free)
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
$ob = unserialize("a:3:{i:0;".serialize($sf).'i:1;r:12;i:2;r:10;}');

var_dump($ob[1]);
?>
--EXPECT--
string(6) "detail"
