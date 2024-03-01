--TEST--
SOAP XML Schema 86: DateTimeInterface date/time types
--EXTENSIONS--
soap
xml
--FILE--
<?php
include "test_schema.inc";
$schema = <<<EOF
    <complexType name="testType">
        <sequence>
            <element name="dateTime" type="dateTime"/>
            <element name="time" type="time"/>
            <element name="date" type="date"/>
            <element name="gYearMonth" type="gYearMonth"/>
            <element name="gYear" type="gYear"/>
            <element name="gMonthDay" type="gMonthDay"/>
            <element name="gDay" type="gDay"/>
            <element name="gMonth" type="gMonth"/>
        </sequence>
    </complexType>
EOF;

$test_dates = [
    new DateTime("2023-10-14 13:37:42.1234+02:00"),
    new DateTimeImmutable("2023-10-14 13:37:42.1234+02:00"),
    new DateTime("2023-10-14 13:37:42.1234Z"),
];

foreach ($test_dates as $date) {
    test_schema($schema,'type="tns:testType"',array(
        'dateTime' => $date,
        'time' => $date,
        'date' => $date,
        'gYearMonth' => $date,
        'gYear' => $date,
        'gMonthDay' => $date,
        'gDay' => $date,
        'gMonth' => $date
    ));
}
echo "ok";
?>
--EXPECTF--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://test-uri/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:test><testParam xsi:type="ns1:testType"><dateTime xsi:type="xsd:dateTime">2023-10-14T13:37:42.123400+02:00</dateTime><time xsi:type="xsd:time">13:37:42.123400+02:00</time><date xsi:type="xsd:date">2023-10-14+02:00</date><gYearMonth xsi:type="xsd:gYearMonth">2023-10+02:00</gYearMonth><gYear xsi:type="xsd:gYear">2023+02:00</gYear><gMonthDay xsi:type="xsd:gMonthDay">--10-14+02:00</gMonthDay><gDay xsi:type="xsd:gDay">---14+02:00</gDay><gMonth xsi:type="xsd:gMonth">--10--+02:00</gMonth></testParam></ns1:test></SOAP-ENV:Body></SOAP-ENV:Envelope>
object(stdClass)#%d (8) {
  ["dateTime"]=>
  string(32) "2023-10-14T13:37:42.123400+02:00"
  ["time"]=>
  string(21) "13:37:42.123400+02:00"
  ["date"]=>
  string(16) "2023-10-14+02:00"
  ["gYearMonth"]=>
  string(13) "2023-10+02:00"
  ["gYear"]=>
  string(10) "2023+02:00"
  ["gMonthDay"]=>
  string(13) "--10-14+02:00"
  ["gDay"]=>
  string(11) "---14+02:00"
  ["gMonth"]=>
  string(12) "--10--+02:00"
}
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://test-uri/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:test><testParam xsi:type="ns1:testType"><dateTime xsi:type="xsd:dateTime">2023-10-14T13:37:42.123400+02:00</dateTime><time xsi:type="xsd:time">13:37:42.123400+02:00</time><date xsi:type="xsd:date">2023-10-14+02:00</date><gYearMonth xsi:type="xsd:gYearMonth">2023-10+02:00</gYearMonth><gYear xsi:type="xsd:gYear">2023+02:00</gYear><gMonthDay xsi:type="xsd:gMonthDay">--10-14+02:00</gMonthDay><gDay xsi:type="xsd:gDay">---14+02:00</gDay><gMonth xsi:type="xsd:gMonth">--10--+02:00</gMonth></testParam></ns1:test></SOAP-ENV:Body></SOAP-ENV:Envelope>
object(stdClass)#9 (8) {
  ["dateTime"]=>
  string(32) "2023-10-14T13:37:42.123400+02:00"
  ["time"]=>
  string(21) "13:37:42.123400+02:00"
  ["date"]=>
  string(16) "2023-10-14+02:00"
  ["gYearMonth"]=>
  string(13) "2023-10+02:00"
  ["gYear"]=>
  string(10) "2023+02:00"
  ["gMonthDay"]=>
  string(13) "--10-14+02:00"
  ["gDay"]=>
  string(11) "---14+02:00"
  ["gMonth"]=>
  string(12) "--10--+02:00"
}
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://test-uri/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:test><testParam xsi:type="ns1:testType"><dateTime xsi:type="xsd:dateTime">2023-10-14T13:37:42.123400Z</dateTime><time xsi:type="xsd:time">13:37:42.123400Z</time><date xsi:type="xsd:date">2023-10-14Z</date><gYearMonth xsi:type="xsd:gYearMonth">2023-10Z</gYearMonth><gYear xsi:type="xsd:gYear">2023Z</gYear><gMonthDay xsi:type="xsd:gMonthDay">--10-14Z</gMonthDay><gDay xsi:type="xsd:gDay">---14Z</gDay><gMonth xsi:type="xsd:gMonth">--10--Z</gMonth></testParam></ns1:test></SOAP-ENV:Body></SOAP-ENV:Envelope>
object(stdClass)#8 (8) {
  ["dateTime"]=>
  string(27) "2023-10-14T13:37:42.123400Z"
  ["time"]=>
  string(16) "13:37:42.123400Z"
  ["date"]=>
  string(11) "2023-10-14Z"
  ["gYearMonth"]=>
  string(8) "2023-10Z"
  ["gYear"]=>
  string(5) "2023Z"
  ["gMonthDay"]=>
  string(8) "--10-14Z"
  ["gDay"]=>
  string(6) "---14Z"
  ["gMonth"]=>
  string(7) "--10--Z"
}
ok
