--TEST--
SOAP 1.2: T38.2 doubleHdr
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$HTTP_RAW_POST_DATA = <<<EOF
<?xml version='1.0' ?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope"> 
  <env:Header>
    <test:echoOk xmlns:test="http://example.org/ts-tests"
          env:mustUnderstand="true" 
          env:role="http://example.org/ts-tests/C">foo</test:echoOk>
    <test:echoOk xmlns:test="http://example.org/ts-tests"
          env:mustUnderstand="1" 
          env:role="http://example.org/ts-tests/C">bar</test:echoOk>
  </env:Header>
  <env:Body>
  </env:Body>
</env:Envelope>
EOF;
include "soap12-test.inc";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope" xmlns:ns1="http://example.org/ts-tests"><env:Header><ns1:responseOk>foo</ns1:responseOk><ns1:responseOk>bar</ns1:responseOk></env:Header><env:Body/></env:Envelope>
ok
