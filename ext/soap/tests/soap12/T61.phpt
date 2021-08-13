--TEST--
SOAP 1.2: T61 countItems
--EXTENSIONS--
soap
--FILE--
<?php
$HTTP_RAW_POST_DATA = <<<EOF
<?xml version='1.0' ?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope"
              xmlns:xsd="http://www.w3.org/2001/XMLSchema"
              xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
  <env:Body>
    <test:countItems xmlns:test="http://example.org/ts-tests"
          xmlns:enc="http://www.w3.org/2003/05/soap-encoding"
          env:encodingStyle="http://www.w3.org/2003/05/soap-encoding">
      <inputStringArray enc:itemType="xsd:string" enc:arraySize="2 *">
        <item xsi:type="xsd:string">hello</item>
        <item xsi:type="xsd:string">world</item>
      </inputStringArray>
    </test:countItems>
  </env:Body>
</env:Envelope>
EOF;
include "soap12-test.inc";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope"><env:Body><env:Fault><env:Code><env:Value>env:Receiver</env:Value></env:Code><env:Reason><env:Text>SOAP-ERROR: Encoding: '*' may only be first arraySize value in list</env:Text></env:Reason></env:Fault></env:Body></env:Envelope>
