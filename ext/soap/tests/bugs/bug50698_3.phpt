--TEST--
Request #50698_3 (SoapClient should handle wsdls with some incompatible endpoints -- EDGECASE: Large set of endpoints all incompatible.)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
try {
    new SoapClient(__DIR__ . '/bug50698_3.wsdl');
    echo "Call: \"new SoapClient(__DIR__.'/bug50698_3.wsdl');\" should throw an exception of type 'SoapFault'";
} catch (SoapFault $e) {
    if ($e->faultcode == 'WSDL' && $e->faultstring == 'SOAP-ERROR: Parsing WSDL: Could not find any usable binding services in WSDL.') {
        echo "ok\n";
    } else {
        echo "Call: \"new SoapClient(__DIR__.'/bug50698_3.wsdl');\" threw a SoapFault with an incorrect faultcode or faultmessage.";
        print_r($e);
    }
}
?>
--EXPECT--
ok
