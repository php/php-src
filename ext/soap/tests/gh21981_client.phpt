--TEST--
GH-21981 (SoapClient: SOAP_USE_DATETIME_OBJECT decodes xsd:dateTime/date/time into DateTimeImmutable)
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

function envelope(string $body): string {
    return <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope
  xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
  xmlns:ns1="urn:test"
  xmlns:xsd="http://www.w3.org/2001/XMLSchema"
  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
 <SOAP-ENV:Body>
  <ns1:fooResponse>{$body}</ns1:fooResponse>
 </SOAP-ENV:Body>
</SOAP-ENV:Envelope>
XML;
}

function show($value): void {
    if (is_object($value)) {
        printf("%s(%s)" . PHP_EOL, get_class($value), $value->format('Y-m-d\\TH:i:s.uP'));
    } else {
        var_dump($value);
    }
}

$opts_off = ['location' => 'test://', 'uri' => 'urn:test'];
$opts_on  = ['location' => 'test://', 'uri' => 'urn:test', 'features' => SOAP_USE_DATETIME_OBJECT];

echo "--- flag OFF: xsd:dateTime stays a string ---" . PHP_EOL;
$c = new StubClient(null, $opts_off);
$c->canned = envelope('<r xsi:type="xsd:dateTime">2026-05-09T12:34:56.123456Z</r>');
show($c->foo());

echo "--- flag ON: xsd:dateTime -> DateTimeImmutable ---" . PHP_EOL;
$c = new StubClient(null, $opts_on);
$c->canned = envelope('<r xsi:type="xsd:dateTime">2026-05-09T12:34:56.123456Z</r>');
show($c->foo());

echo "--- flag ON: xsd:dateTime with offset ---" . PHP_EOL;
$c = new StubClient(null, $opts_on);
$c->canned = envelope('<r xsi:type="xsd:dateTime">2026-05-09T12:34:56+02:00</r>');
show($c->foo());

echo "--- flag ON: xsd:date -> DateTimeImmutable ---" . PHP_EOL;
$c = new StubClient(null, $opts_on);
$c->canned = envelope('<r xsi:type="xsd:date">2026-05-09</r>');
show($c->foo());

echo "--- flag ON: xsd:time -> DateTimeImmutable ---" . PHP_EOL;
$c = new StubClient(null, $opts_on);
$c->canned = envelope('<r xsi:type="xsd:time">12:34:56Z</r>');
show($c->foo());

echo "--- flag ON: malformed dateTime -> graceful string fallback ---" . PHP_EOL;
$c = new StubClient(null, $opts_on);
$c->canned = envelope('<r xsi:type="xsd:dateTime">not-a-date</r>');
show($c->foo());

echo "--- flag ON: xsi:nil -> NULL ---" . PHP_EOL;
$c = new StubClient(null, $opts_on);
$c->canned = envelope('<r xsi:type="xsd:dateTime" xsi:nil="true"/>');
show($c->foo());
?>
--EXPECTF--
--- flag OFF: xsd:dateTime stays a string ---
string(27) "2026-05-09T12:34:56.123456Z"
--- flag ON: xsd:dateTime -> DateTimeImmutable ---
DateTimeImmutable(2026-05-09T12:34:56.123456+00:00)
--- flag ON: xsd:dateTime with offset ---
DateTimeImmutable(2026-05-09T12:34:56.000000+02:00)
--- flag ON: xsd:date -> DateTimeImmutable ---
DateTimeImmutable(2026-05-09T00:00:00.000000%s)
--- flag ON: xsd:time -> DateTimeImmutable ---
DateTimeImmutable(%s12:34:56.000000+00:00)
--- flag ON: malformed dateTime -> graceful string fallback ---
string(10) "not-a-date"
--- flag ON: xsi:nil -> NULL ---
NULL
