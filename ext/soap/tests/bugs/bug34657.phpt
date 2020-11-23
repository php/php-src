--TEST--
Bug #34657 (If you get a communication problem when loading the WSDL, it fatal's)
--SKIPIF--
<?php
require_once('skipif.inc');
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
SoapFault
SOAP-ERROR: Parsing WSDL: Couldn't load from 'http://i_dont_exist.com/some.wsdl'%A
ok
