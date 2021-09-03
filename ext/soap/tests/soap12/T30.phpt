--TEST--
SOAP 1.2: T30 echoOk
--EXTENSIONS--
soap
--FILE--
<?php
$HTTP_RAW_POST_DATA = <<<EOF
<?xml version='1.0' ?>
<env:Envelope xmlns:env="http://schemas.xmlsoap.org/soap/envelope/">
  <env:Body>
    <test:echoOk xmlns:test="http://example.org/ts-tests">foo</test:echoOk>
  </env:Body>
</env:Envelope>
EOF;
include "soap12-test.inc";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://example.org/ts-tests"><SOAP-ENV:Body><ns1:responseOk>foo</ns1:responseOk></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
