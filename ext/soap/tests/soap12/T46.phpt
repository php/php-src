--TEST--
SOAP 1.2: T46 echoNestedArray
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$HTTP_RAW_POST_DATA = <<<EOF
<?xml version="1.0"?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope"
              xmlns:xsd="http://www.w3.org/2001/XMLSchema"
              xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
  <env:Body>
    <test:echoNestedArray xmlns:test="http://exaple.org/ts-tests"
          env:encodingStyle="http://www.w3.org/2003/05/soap-encoding">
      <inputStruct xsi:type="ns1:SOAPArrayStruct"
                   xmlns:ns1="http://example.org/ts-tests/xsd">
        <varInt xsi:type="xsd:int">42</varInt>
        <varFloat xsi:type="xsd:float">0.005</varFloat>
        <varString xsi:type="xsd:string">hello world</varString>
        <varArray enc:itemType="xsd:string" enc:arraySize="3"
		          xmlns:enc="http://www.w3.org/2003/05/soap-encoding">
          <item xsi:type="xsd:string">red</item>
          <item xsi:type="xsd:string">blue</item>
          <item xsi:type="xsd:string">green</item>
        </varArray>
      </inputStruct>
    </test:echoNestedArray>
  </env:Body>
</env:Envelope>
EOF;
include "soap12-test.inc";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope" xmlns:ns1="http://example.org/ts-tests" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:enc="http://www.w3.org/2003/05/soap-encoding" xmlns:ns2="http://example.org/ts-tests/xsd"><env:Body xmlns:rpc="http://www.w3.org/2003/05/soap-rpc"><ns1:echoNestedArrayResponse env:encodingStyle="http://www.w3.org/2003/05/soap-encoding"><rpc:result>return</rpc:result><return xsi:type="ns2:SOAPArrayStruct"><varString xsi:type="xsd:string">hello world</varString><varInt xsi:type="xsd:int">42</varInt><varFloat xsi:type="xsd:float">0.005</varFloat><varArray enc:itemType="xsd:string" enc:arraySize="3" xsi:type="ns2:ArrayOfstring"><item xsi:type="xsd:string">red</item><item xsi:type="xsd:string">blue</item><item xsi:type="xsd:string">green</item></varArray></return></ns1:echoNestedArrayResponse></env:Body></env:Envelope>
ok
