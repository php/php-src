--TEST--
GH-21981 (User typemap from_xml still wins over SOAP_USE_DATETIME_OBJECT)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
class StubClient extends SoapClient {
    public string $canned;
    public function __doRequest(string $request, string $location, string $action, int $version, bool $oneWay = false, ?string $uriParserClass = null): ?string {
        return $this->canned;
    }
}

function from_xml_dt(string $xml): string {
    return 'typemap-handled:' . $xml;
}

$canned = <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope
  xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
  xmlns:ns1="urn:test"
  xmlns:xsd="http://www.w3.org/2001/XMLSchema"
  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
 <SOAP-ENV:Body>
  <ns1:fooResponse><r xsi:type="xsd:dateTime">2026-05-09T12:34:56.123456Z</r></ns1:fooResponse>
 </SOAP-ENV:Body>
</SOAP-ENV:Envelope>
XML;

$opts = [
    'location' => 'test://',
    'uri'      => 'urn:test',
    'features' => SOAP_USE_DATETIME_OBJECT,
    'typemap'  => [[
        'type_ns'   => 'http://www.w3.org/2001/XMLSchema',
        'type_name' => 'dateTime',
        'from_xml'  => 'from_xml_dt',
    ]],
];

$c = new StubClient(null, $opts);
$c->canned = $canned;
var_dump($c->foo());
?>
--EXPECTF--
string(%d) "typemap-handled:<r xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:type="xsd:dateTime"%A>2026-05-09T12:34:56.123456Z</r>%A"
