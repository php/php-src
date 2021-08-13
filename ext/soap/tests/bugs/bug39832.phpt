--TEST--
Bug #39832 (SOAP Server: parameter not matching the WSDL specified type are set to 0)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$HTTP_RAW_POST_DATA = <<<EOF
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://test.pl"><SOAP-ENV:Body>
<SOAP-ENV:Test>
<parameters priority="high">
<ns1:NetworkErrorCode>1</ns1:NetworkErrorCode>
</parameters>
</SOAP-ENV:Test></SOAP-ENV:Body></SOAP-ENV:Envelope>
EOF;

function Test($x) {
  return $x->priority;
}

$x = new SoapServer(__DIR__."/bug39832.wsdl");
$x->addFunction("Test");
$x->handle($HTTP_RAW_POST_DATA);
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>SOAP-ERROR: Encoding: Violation of encoding rules</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
