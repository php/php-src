--TEST--
GH-21981 (SoapServer: SOAP_USE_DATETIME_OBJECT decodes incoming xsd:dateTime arguments into DateTimeImmutable)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
function consume($d) {
    if (is_object($d)) {
        return get_class($d) . '(' . $d->format('Y-m-d\\TH:i:s.uP') . ')';
    }
    return gettype($d) . '(' . var_export($d, true) . ')';
}

$request = <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope
  SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"
  xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
  xmlns:xsd="http://www.w3.org/2001/XMLSchema"
  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
 <SOAP-ENV:Body>
  <ns1:consume xmlns:ns1="urn:test">
    <when xsi:type="xsd:dateTime">2026-05-09T12:34:56.123456+02:00</when>
  </ns1:consume>
 </SOAP-ENV:Body>
</SOAP-ENV:Envelope>
XML;

echo "--- flag OFF: handler receives a string ---" . PHP_EOL;
$server = new SoapServer(null, ['uri' => 'urn:test']);
$server->addFunction('consume');
$server->handle($request);

echo "--- flag ON: handler receives a DateTimeImmutable ---" . PHP_EOL;
$server = new SoapServer(null, ['uri' => 'urn:test', 'features' => SOAP_USE_DATETIME_OBJECT]);
$server->addFunction('consume');
$server->handle($request);
?>
--EXPECT--
--- flag OFF: handler receives a string ---
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="urn:test" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:consumeResponse><return xsi:type="xsd:string">string('2026-05-09T12:34:56.123456+02:00')</return></ns1:consumeResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
--- flag ON: handler receives a DateTimeImmutable ---
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="urn:test" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:consumeResponse><return xsi:type="xsd:string">DateTimeImmutable(2026-05-09T12:34:56.123456+02:00)</return></ns1:consumeResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
