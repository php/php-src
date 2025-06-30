--TEST--
SOAP Server: user fault with language
--EXTENSIONS--
soap
--FILE--
<?php
function test() {
    global $server;
    $server->fault("MyFault", "My fault string", lang: "custom");
}

$server = new SoapServer(null, ['uri' => 'http://testuri.org', 'soap_version' => SOAP_1_2]);
$server->addFunction("test");

$HTTP_RAW_POST_DATA = <<<EOF
<?xml version="1.0" encoding="ISO-8859-1"?>
<SOAP-ENV:Envelope
  xmlns:SOAP-ENV="http://www.w3.org/2003/05/soap-envelope">
  <SOAP-ENV:Body>
    <ns1:test xmlns:ns1="http://testuri.org"/>
  </SOAP-ENV:Body>
</SOAP-ENV:Envelope>
EOF;

$server->handle($HTTP_RAW_POST_DATA);
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope"><env:Body><env:Fault><env:Code><env:Value>MyFault</env:Value></env:Code><env:Reason><env:Text xml:lang="custom">My fault string</env:Text></env:Reason></env:Fault></env:Body></env:Envelope>
