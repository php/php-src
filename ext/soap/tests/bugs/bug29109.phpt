--TEST--
Bug #29109 (Uncaught SoapFault exception: [WSDL] Out of memory)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$client = new SoapClient(__DIR__."/bug29109.wsdl");
var_dump($client->__getFunctions());
?>
--EXPECT--
array(1) {
  [0]=>
  string(53) "HelloWorldResponse HelloWorld(HelloWorld $parameters)"
}
