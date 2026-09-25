--TEST--
GH-22984 (Native stack overflow while a SoapServer decodes a cyclic href reference)
--EXTENSIONS--
soap
--FILE--
<?php
$decoded = null;

function probe($a) {
    global $decoded;
    $decoded = $a;
    return 1;
}

$request = <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://testuri.org" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
  <SOAP-ENV:Body>
    <ns1:probe>
      <a id="a" SOAP-ENC:arrayType="xsd:anyType[1]">
        <item href="#a"/>
      </a>
    </ns1:probe>
  </SOAP-ENV:Body>
</SOAP-ENV:Envelope>
XML;

$server = new SoapServer(null, ['uri' => 'http://testuri.org']);
$server->addFunction('probe');
$server->handle($request);
var_dump($decoded);
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://testuri.org" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:probeResponse><return xsi:type="xsd:int">1</return></ns1:probeResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
array(1) {
  [0]=>
  array(0) {
  }
}
