--TEST--
load_wsdl_ex() leaks the import location if imported WSDL fails to load
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php

try {
	$client = new SoapClient(__DIR__ . '/load_wsdl_ex-failure-leak.wsdl');
} catch (SoapFault $e) {
	echo get_class($e) . ': ' . $e->getMessage() . "\n";
}

?>
--EXPECTF--
SoapFault: SOAP-ERROR: Parsing WSDL: Couldn't load from '%sload_wsdl_ex-failure-leak-missing.wsdl' : %s
