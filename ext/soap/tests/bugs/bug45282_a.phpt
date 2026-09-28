--TEST--
Bug #45282 (SoapClient has namespace issues when WSDL is distributed)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$client = new SoapClient(__DIR__ . "/bug45282_a.wsdl");
var_dump($client->__getFunctions());
?>
--EXPECT--
array(3) {
  [0]=>
  string(34) "date reportHealthView(int $number)"
  [1]=>
  string(34) "void reportHealthView(string $str)"
  [2]=>
  string(34) "date reportHealthView(int $number)"
}
