--TEST--
SOAP: DOCTYPE named like the envelope is not taken for it when rejecting a request
--EXTENSIONS--
soap
--FILE--
<?php
function test($x) {
    return $x;
}

$request = <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE Envelope [<!NOTATION n SYSTEM "x">]>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope" xmlns:ns1="urn:test">
  <env:Body>
    <ns1:test><param0>x</param0></ns1:test>
  </env:Body>
</env:Envelope>
XML;

$server = new SoapServer(null, ['uri' => 'urn:test']);
$server->addFunction('test');
$server->handle($request);
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope"><env:Body><env:Fault><env:Code><env:Value>env:Receiver</env:Value></env:Code><env:Reason><env:Text>DTD are not supported by SOAP</env:Text></env:Reason></env:Fault></env:Body></env:Envelope>
