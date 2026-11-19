--TEST--
SOAP: DOCTYPE named like the envelope is not taken for it when rejecting a posted request
--POST--
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE Envelope [<!NOTATION n SYSTEM "x">]>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="urn:test">
  <SOAP-ENV:Body>
    <ns1:test><param0>x</param0></ns1:test>
  </SOAP-ENV:Body>
</SOAP-ENV:Envelope>
--EXTENSIONS--
soap
--FILE--
<?php
function test($x) {
    return $x;
}

$server = new SoapServer(null, ['uri' => 'urn:test']);
$server->addFunction('test');
$server->handle();
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>DTD are not supported by SOAP</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
