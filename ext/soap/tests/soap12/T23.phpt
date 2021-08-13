--TEST--
SOAP 1.2: T23 echoOk
--EXTENSIONS--
soap
--FILE--
<?php
$HTTP_RAW_POST_DATA = <<<EOF
<?xml version='1.0' ?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope">
  <env:Header>
    <test:Unknown xmlns:test="http://example.org/ts-tests"
          env:mustUnderstand="1">foo</test:Unknown>
    <test:echoOk xmlns:test="http://example.org/ts-tests"
          env:mustUnderstand="wrong">foo</test:echoOk>
  </env:Header>
  <env:Body>
  </env:Body>
</env:Envelope>
EOF;
include "soap12-test.inc";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope"><env:Body><env:Fault><env:Code><env:Value>env:Sender</env:Value></env:Code><env:Reason><env:Text>mustUnderstand value is not boolean</env:Text></env:Reason></env:Fault></env:Body></env:Envelope>
