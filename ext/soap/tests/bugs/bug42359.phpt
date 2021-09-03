--TEST--
Bug #42326 (SoapServer crash)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$soap = new SoapClient(__DIR__."/bug42359.wsdl");
print_r($soap->__getTypes());
?>
--EXPECT--
Array
(
    [0] => list listItem {anonymous1}
    [1] => string anonymous1
    [2] => string enumItem
    [3] => list listItem2 {enumItem}
)
