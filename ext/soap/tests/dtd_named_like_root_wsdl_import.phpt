--TEST--
SOAP: DOCTYPE named like the schema root element is not taken for it in a WSDL import
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$client = new SoapClient(__DIR__ . '/dtd_named_like_root_wsdl_import.wsdl');
var_dump($client->__getTypes());
?>
--EXPECT--
array(1) {
  [0]=>
  string(30) "struct Imported {
 string a;
}"
}
