--TEST--
SOAP Server 24: Send SOAP headers those were not received
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
class TestHeader1 extends SoapHeader {
	function __construct($data) {
		parent::__construct("http://testuri.org", "Test1", $data);
	}
}

class TestHeader2 extends SoapHeader {
	function __construct($data) {
		parent::__construct("http://testuri.org", "Test2", $data);
	}
}

function test() {
	global $server;
	$server->addSoapHeader(new TestHeader1("Hello Header!"));
	$server->addSoapHeader(new TestHeader2("Hello Header!"));
	return "Hello Body!";
}

$server = new soapserver(null,array('uri'=>"http://testuri.org"));
$server->addfunction("test");

$HTTP_RAW_POST_DATA = <<<EOF
<?xml version="1.0" encoding="ISO-8859-1"?>
<SOAP-ENV:Envelope
  SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"
  xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
  xmlns:xsd="http://www.w3.org/2001/XMLSchema"
  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
  xmlns:si="http://soapinterop.org/xsd">
  <SOAP-ENV:Body>
    <ns1:test xmlns:ns1="http://testuri.org"/>
  </SOAP-ENV:Body>
</SOAP-ENV:Envelope>
EOF;

$server->handle($HTTP_RAW_POST_DATA);
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://testuri.org" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Header><ns1:Test1>Hello Header!</ns1:Test1><ns1:Test2>Hello Header!</ns1:Test2></SOAP-ENV:Header><SOAP-ENV:Body><ns1:testResponse><return xsi:type="xsd:string">Hello Body!</return></ns1:testResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
