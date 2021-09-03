--TEST--
Bug #31755 (Cannot create SOAP header in no namespace)
--EXTENSIONS--
soap
--FILE--
<?php
class MySoapClient extends SoapClient {
    public function __doRequest($request, $location, $action, $version, $one_way = 0): string {
        echo $request, "\n";
        return '';
    }
}
$client = new MySoapClient(null, array(
    'location' => 'http://localhost', 'uri' => 'myNS', 'exceptions' => false
));

try {
    new SOAPHeader('', 'foo', 'bar');
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

$header = new SOAPHeader('namespace', 'foo', 'bar');
$response= $client->__soapCall('function', array(), null, $header);

print $client->__getLastRequest();
?>
--EXPECT--
SoapHeader::__construct(): Argument #1 ($namespace) cannot be empty
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="myNS" xmlns:ns2="namespace" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Header><ns2:foo>bar</ns2:foo></SOAP-ENV:Header><SOAP-ENV:Body><ns1:function/></SOAP-ENV:Body></SOAP-ENV:Envelope>
