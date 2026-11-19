--TEST--
SOAP: DOCTYPE named like the schema root element is not taken for it
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$client = new SoapClient(__DIR__ . '/dtd_named_like_root_schema.wsdl');
var_dump($client->__getTypes());
?>
--EXPECT--
array(2) {
  [0]=>
  string(30) "struct Imported {
 string a;
}"
  [1]=>
  string(30) "struct Included {
 string b;
}"
}
