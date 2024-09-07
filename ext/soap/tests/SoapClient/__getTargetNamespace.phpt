--TEST--
Request #61495 (SoapClient::__getTargetNamespace)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$client = new SoapClient(__DIR__.'/../bugs/bug28985.wsdl');
var_dump($client->__getTargetNamespace());

$client = new SoapClient(__DIR__.'/../no_target_ns.wsdl');
var_dump($client->__getTargetNamespace());

$client = new SoapClient(null, ['location' => 'http://tmp', 'uri' => 'http://tmp']);
var_dump($client->__getTargetNamespace());
?>
--EXPECT--
string(19) "http://tempuri.org/"
NULL
NULL
