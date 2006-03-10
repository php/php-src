--TEST--
SOAP 1.2: T63 validateCountryCode
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$HTTP_RAW_POST_DATA = <<<EOF
<?xml version='1.0' ?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope"> 
  <env:Header>
    <test:validateCountryCode xmlns:test="http://example.org/ts-tests"
          env:role="http://example.org/ts-tests/C"
          env:mustUnderstand="1">ABCD</test:validateCountryCode>
  </env:Header>
  <env:Body>
  </env:Body>
</env:Envelope>

EOF;
include "soap12-test.inc";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope" xmlns:ns1="http://example.org/ts-tests"><env:Header><ns1:validateCountryCodeFault>Country code must be 2 letters.</ns1:validateCountryCodeFault></env:Header><env:Body><env:Fault><env:Code><env:Value>env:Sender</env:Value></env:Code><env:Reason><env:Text>Not a valid country code</env:Text></env:Reason></env:Fault></env:Body></env:Envelope>
ok
