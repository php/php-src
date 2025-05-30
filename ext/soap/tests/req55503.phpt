--TEST--
Request #55503 (Extend __getTypes to support enumerations)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$client = new SoapClient(__DIR__.'/req55503.wsdl');
var_dump($client->__getTypes());
?>
--EXPECT--
array(1) {
  [0]=>
  string(102) "anyType PersonaMemberType {NEW, LIMITED, FREE, PAID_ACTIVE, TRIAL_ACTIVE, PAID_EXPIRED, TRIAL_EXPIRED}"
}
