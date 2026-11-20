--TEST--
GHSA-4646-p932-gw34: DTD entity references must not outlive their declarations
--POST--
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE Envelope [<!ENTITY e "boom">]>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
                   xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                   xmlns:xsd="http://www.w3.org/2001/XMLSchema"
                   xmlns:ns1="urn:t">
  <SOAP-ENV:Header>
    <h SOAP-ENV:actor="&e;" SOAP-ENV:mustUnderstand="1"/>
  </SOAP-ENV:Header>
  <SOAP-ENV:Body>
    <ns1:echo_><param0 xsi:type="xsd:string">x</param0></ns1:echo_>
  </SOAP-ENV:Body>
</SOAP-ENV:Envelope>
--EXTENSIONS--
soap
--FILE--
<?php
function echo_($x) {
    return $x;
}

$server = new SoapServer(null, [
    'uri' => 'urn:t',
    'style' => SOAP_RPC,
    'use' => SOAP_ENCODED,
]);
$server->addFunction('echo_');
$server->handle();
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>DTD are not supported by SOAP</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
