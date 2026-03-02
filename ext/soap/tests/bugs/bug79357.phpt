--TEST--
Bug #79357: SOAP request segfaults when any request parameter is missing
--EXTENSIONS--
soap
--FILE--
<?php

$sc = new SoapClient(__DIR__ . '/bug79357.wsdl');
$res = $sc->Add(['intA'=>1]);
var_dump($res);

?>
--EXPECTF--
Fatal error: Uncaught SoapFault exception: [Client] SOAP-ERROR: Encoding: object has no 'intB' property in %s:%d
Stack trace:
#0 %s(%d): SoapClient->__call('Add', Array)
#1 {main}
  thrown in %s on line %d
