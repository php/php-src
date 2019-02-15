--TEST--
SOAP 1.2: T27 echoStringArray
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$HTTP_RAW_POST_DATA = <<<EOF
<?xml version='1.0' ?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope"
              xmlns:xs="http://www.w3.org/2001/XMLSchema">
  <env:Body>
    <test:echoStringArray xmlns:test="http://example.org/ts-tests" 
          xmlns:enc="http://www.w3.org/2003/05/soap-encoding"
          env:encodingStyle="http://www.w3.org/2003/05/soap-encoding">
      <test:array enc:itemType="xs:string" enc:arraySize="1">
        <a>
          <b>1</b>
        </a>
      </test:array>		
    </test:echoStringArray>
 </env:Body>
</env:Envelope>
EOF;
include "soap12-test.inc";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope"><env:Body><env:Fault><env:Code><env:Value>env:Receiver</env:Value></env:Code><env:Reason><env:Text>SOAP-ERROR: Encoding: Violation of encoding rules</env:Text></env:Reason></env:Fault></env:Body></env:Envelope>
