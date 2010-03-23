--TEST--
SOAP XML Schema 64: standard date/time types
--SKIPIF--
<?php require_once('skipif.inc'); ?>
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
$date = gmmktime(1,2,3,4,5,1976);
putenv('TZ=GMT');
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
echo "ok";
?>
--EXPECTF--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://test-uri/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:test><testParam xsi:type="ns1:testType"><dateTime xsi:type="xsd:dateTime">1976-04-05T01:02:03Z</dateTime><time xsi:type="xsd:time">01:02:03Z</time><date xsi:type="xsd:date">1976-04-05Z</date><gYearMonth xsi:type="xsd:gYearMonth">1976-04Z</gYearMonth><gYear xsi:type="xsd:gYear">1976Z</gYear><gMonthDay xsi:type="xsd:gMonthDay">--04-05Z</gMonthDay><gDay xsi:type="xsd:gDay">---05Z</gDay><gMonth xsi:type="xsd:gMonth">--04--Z</gMonth></testParam></ns1:test></SOAP-ENV:Body></SOAP-ENV:Envelope>
object(stdClass)#%d (8) {
  ["dateTime"]=>
  string(20) "1976-04-05T01:02:03Z"
  ["time"]=>
  string(9) "01:02:03Z"
  ["date"]=>
  string(11) "1976-04-05Z"
  ["gYearMonth"]=>
  string(8) "1976-04Z"
  ["gYear"]=>
  string(5) "1976Z"
  ["gMonthDay"]=>
  string(8) "--04-05Z"
  ["gDay"]=>
  string(6) "---05Z"
  ["gMonth"]=>
  string(7) "--04--Z"
}
ok
