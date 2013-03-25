--TEST--
Bug #34657 (If you get a communication problem when loading the WSDL, it fatal's)
--SKIPIF--
<?php 
require_once('skipif.inc'); 
if (extension_loaded("openssl")) {
	/* 
	 when openssl loaded, tcp stream is less verbose, so some error messages are missing 
	 so let's skip the test in this case  
	 */
	die("skip OpenSSL extension required");
}
?>
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
?>
--EXPECTF--
Warning: SoapClient::SoapClient(): %s %sbug34657.php on line 3

Warning: SoapClient::SoapClient(http://i_dont_exist.com/some.wsdl): failed to open stream: %sbug34657.php on line 3

Warning: SoapClient::SoapClient(): I/O warning : failed to load external entity "http://i_dont_exist.com/some.wsdl" in %sbug34657.php on line 3
SoapFault
SOAP-ERROR: Parsing WSDL: Couldn't load from 'http://i_dont_exist.com/some.wsdl'%S
ok
