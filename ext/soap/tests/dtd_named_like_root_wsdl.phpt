--TEST--
SOAP: DOCTYPE named like the WSDL root element is not taken for it
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$client = new SoapClient(__DIR__ . '/dtd_named_like_root_wsdl.wsdl');
var_dump($client->__getFunctions());
?>
--EXPECT--
array(1) {
  [0]=>
  string(22) "string echo(string $p)"
}
