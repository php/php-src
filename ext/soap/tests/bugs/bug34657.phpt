--TEST--
Bug #34657 (If you get a communication problem when loading the WSDL, it fatal's)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
try {
	$client = new SoapClient('http://i_dont_exist.com/some.wsdl');
	echo "?\n";
} catch (SoapFault $e) {
	echo get_class($e)."\n";
	echo $e->faultstring."\n";
	echo "ok\n";
} catch (Exception $e) {
	echo get_class($e)."\n";
}
--EXPECTF--
Warning: SoapClient::__construct(): php_network_getaddresses: getaddrinfo failed: %sbug34657.php on line 3

Warning: SoapClient::__construct(http://i_dont_exist.com/some.wsdl): failed to open stream: %sbug34657.php on line 3

Warning: SoapClient::__construct(): I/O warning : failed to load external entity "http://i_dont_exist.com/some.wsdl" in %sbug34657.php on line 3
SoapFault
SOAP-ERROR: Parsing WSDL: Couldn't load from 'http://i_dont_exist.com/some.wsdl'
ok
