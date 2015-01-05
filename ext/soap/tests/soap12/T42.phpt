--TEST--
SOAP 1.2: T42 echoStructArray
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
precision=14
--FILE--
<?php
$HTTP_RAW_POST_DATA = <<<EOF
<?xml version="1.0"?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope"
              xmlns:xsd="http://www.w3.org/2001/XMLSchema"
              xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
  <env:Body>
    <test:echoStructArray xmlns:test="http://example.org/ts-tests"
          env:encodingStyle="http://www.w3.org/2003/05/soap-encoding">
      <inputStructArray enc:itemType="ns1:SOAPStruct"
                        enc:arraySize="2"
                        xmlns:ns1="http://example.org/ts-tests/xsd"
                        xmlns:enc="http://www.w3.org/2003/05/soap-encoding">
        <item xsi:type="ns1:SOAPStruct">
          <varInt xsi:type="xsd:int">42</varInt>
          <varFloat xsi:type="xsd:float">0.005</varFloat>
          <varString xsi:type="xsd:string">hello world</varString>
        </item>
        <item xsi:type="ns1:SOAPStruct">
          <varInt xsi:type="xsd:int">43</varInt>
          <varFloat xsi:type="xsd:float">0.123</varFloat>
          <varString xsi:type="xsd:string">bye world</varString>
        </item>
      </inputStructArray>
    </test:echoStructArray>
  </env:Body>
</env:Envelope>
EOF;
include "soap12-test.inc";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope" xmlns:ns1="http://example.org/ts-tests" xmlns:ns2="http://example.org/ts-tests/xsd" xmlns:enc="http://www.w3.org/2003/05/soap-encoding" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"><env:Body xmlns:rpc="http://www.w3.org/2003/05/soap-rpc"><ns1:echoStructArrayResponse env:encodingStyle="http://www.w3.org/2003/05/soap-encoding"><rpc:result>return</rpc:result><return enc:itemType="ns2:SOAPStruct" enc:arraySize="2" xsi:type="ns2:ArrayOfSOAPStruct"><item xsi:type="ns2:SOAPStruct"><varString xsi:type="xsd:string">hello world</varString><varInt xsi:type="xsd:int">42</varInt><varFloat xsi:type="xsd:float">0.005</varFloat></item><item xsi:type="ns2:SOAPStruct"><varString xsi:type="xsd:string">bye world</varString><varInt xsi:type="xsd:int">43</varInt><varFloat xsi:type="xsd:float">0.123</varFloat></item></return></ns1:echoStructArrayResponse></env:Body></env:Envelope>
ok
