--TEST--
GH-22984 (Native stack overflow while decoding a cyclic SOAP 1.1 href reference)
--EXTENSIONS--
soap
--FILE--
<?php
class TestSoapClient extends SoapClient {
    public function __doRequest($request, $location, $action, $version, $one_way = false): ?string {
        return <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://testuri.org" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
  <SOAP-ENV:Body>
    <ns1:probeResponse>
      <ret id="a" SOAP-ENC:arrayType="xsd:anyType[1]">
        <item href="#a"/>
      </ret>
    </ns1:probeResponse>
  </SOAP-ENV:Body>
</SOAP-ENV:Envelope>
XML;
    }
}

$client = new TestSoapClient(null, ['location' => 'test://', 'uri' => 'http://testuri.org', 'soap_version' => SOAP_1_1]);
var_dump($client->__soapCall('probe', []));
?>
--EXPECT--
array(1) {
  [0]=>
  array(0) {
  }
}
