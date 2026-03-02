--TEST--
Bug #80672 Null Dereference in SoapClient
--EXTENSIONS--
soap
--FILE--
<?php
try {
    $client = new SoapClient(__DIR__ . "/bug80672.xml");
    $query = $soap->query(array('sXML' => 'something'));
} catch(SoapFault $e) {
    print $e->getMessage();
}
?>
--EXPECT--
SOAP-ERROR: Parsing WSDL: Unexpected WSDL element <>
