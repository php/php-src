--TEST--
Bug #29109 (Uncaught SoapFault exception: [WSDL] Out of memory)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$client = new SoapClient(dirname(__FILE__)."/bug29109.wsdl");
var_dump($client->__getFunctions()); 
?>
--EXPECT--
array(3) {
  [0]=>
  string(53) "HelloWorldResponse HelloWorld(HelloWorld $parameters)"
  [1]=>
  string(19) "string HelloWorld()"
  [2]=>
  string(19) "string HelloWorld()"
}