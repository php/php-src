--TEST--
Bug #38536 (SOAP returns an array of values instead of an object)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
class LocalSoapClient extends SoapClient {
  function __doRequest($request, $location, $action, $version) {
    return <<<EOF
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope
  SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"
  xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/"
  xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
  xmlns:xsd="http://www.w3.org/2001/XMLSchema"
  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
  xmlns:ns1="http://www.grupos.com.br/ws/enturma/client">
<SOAP-ENV:Body>
<getClientInfoFromDomainResponse SOAP-ENC:root="1">
  <xsd:Result xsi:type="ns1:ClientType">
    <id xsi:type="xsd:int">2</id>
    <address href="#i2"/>
  </xsd:Result>
</getClientInfoFromDomainResponse>
<xsd:address id="i2" xsi:type="ns1:ClientAddressType" SOAP-ENC:root="0">
  <idClient xsi:type="xsd:long">2</idClient>
  <address href="#i3"/>
</xsd:address>
<address xsi:type="xsd:string" id="i3" SOAP-ENC:root="0">Test</address>
</SOAP-ENV:Body>
</SOAP-ENV:Envelope>
EOF;
  }
}

ini_set("soap.wsdl_cache_enabled", 0);
$SOAPObject = new LocalSoapClient(dirname(__FILE__).'/bug38536.wsdl');
print_r($SOAPObject->test());
?>
--EXPECT--
stdClass Object
(
    [id] => 2
    [address] => stdClass Object
        (
            [idClient] => 2
            [address] => Test
        )

)
