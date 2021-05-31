--TEST--
Bug #73452 Segfault (Regression for #69152)
--EXTENSIONS--
soap
--FILE--
<?php

$data = 'O:9:"SoapFault":4:{s:9:"faultcode";i:4298448493;s:11:"faultstring";i:4298448543;s:7:"'."\0*\0".'file";s:0:"";s:7:"'."\0*\0".'line";i:0;}';
echo unserialize($data);

?>
==DONE==
--EXPECTF--
SoapFault exception: [4298448493] 4298448543 in :0
Stack trace:
#0 %sbug73452.php(4): unserialize('O:9:"SoapFault"...')
#1 {main}==DONE==
