--TEST--
Bug #30175 (SOAP results aren't parsed correctly)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php

class LocalSoapClient extends SoapClient {

  function __doRequest($request, $location, $action, $version, $one_way = 0) {
    return <<<EOF
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope
xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/"
xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
xmlns:xsd="http://www.w3.org/2001/XMLSchema"
xmlns:ns1="urn:qweb">
<SOAP-ENV:Body
SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"
id="_0">
<ns1:HostInfo xsi:type="ns1:HostInfo">
<name xsi:type="xsd:string">blah blah some name field</name>
<shortDescription xsi:type="xsd:string">This is a description. more blah blah blah</shortDescription>
<ipAddress xsi:type="xsd:string">127.0.0.1</ipAddress>
</ns1:HostInfo>
</SOAP-ENV:Body>
</SOAP-ENV:Envelope>
EOF;
  }

}

$client = new LocalSoapClient(__DIR__."/bug30175.wsdl");
var_dump($client->qwebGetHostInfo());
?>
--EXPECT--
array(3) {
  ["name"]=>
  string(25) "blah blah some name field"
  ["shortDescription"]=>
  string(42) "This is a description. more blah blah blah"
  ["ipAddress"]=>
  string(9) "127.0.0.1"
}
