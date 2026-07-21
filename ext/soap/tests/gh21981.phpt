--TEST--
GH-21981 (Returning a bare DateTimeInterface from a SoapServer should not require SoapVar)
--EXTENSIONS--
soap
--FILE--
<?php
function getCurrentDate() {
    return new DateTime("2026-05-09T12:34:56.123456+00:00");
}

function getCurrentDateImmutable() {
    return new DateTimeImmutable("2026-05-09T12:34:56.123456+00:00");
}

$request_wsdl = <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
  xmlns:ns1="http://localhost"
  xmlns:xsd="http://www.w3.org/2001/XMLSchema"
  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
  xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/"
  SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
 <SOAP-ENV:Body>
  <ns1:getCurrentDate/>
 </SOAP-ENV:Body>
</SOAP-ENV:Envelope>
XML;

echo "--- WSDL with unresolved part type (xsd:datetime typo) ---\n";
$server = new SoapServer(__DIR__.'/gh21981.wsdl', ['cache_wsdl' => WSDL_CACHE_NONE]);
$server->addFunction('getCurrentDate');
$server->handle($request_wsdl);

$request_nowsdl = <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope
  SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"
  xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
  xmlns:xsd="http://www.w3.org/2001/XMLSchema"
  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
 <SOAP-ENV:Body>
  <ns1:getCurrentDateImmutable xmlns:ns1="http://testuri.org" />
 </SOAP-ENV:Body>
</SOAP-ENV:Envelope>
XML;

echo "--- Non-WSDL server, DateTimeImmutable return ---\n";
$server = new SoapServer(null, ['uri' => 'http://testuri.org']);
$server->addFunction('getCurrentDateImmutable');
$server->handle($request_nowsdl);
?>
--EXPECT--
--- WSDL with unresolved part type (xsd:datetime typo) ---
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="urn:xmethods-delayed-quotes" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:getCurrentDateResponse><currentDate xsi:type="xsd:dateTime">2026-05-09T12:34:56.123456Z</currentDate></ns1:getCurrentDateResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
--- Non-WSDL server, DateTimeImmutable return ---
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://testuri.org" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:getCurrentDateImmutableResponse><return xsi:type="xsd:dateTime">2026-05-09T12:34:56.123456Z</return></ns1:getCurrentDateImmutableResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
