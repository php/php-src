--TEST--
SOAP 1.2: T50 echoIntegerArray
--EXTENSIONS--
soap
--FILE--
<?php
$HTTP_RAW_POST_DATA = <<<EOF
<?xml version="1.0"?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope"
              xmlns:xsd="http://www.w3.org/2001/XMLSchema"
              xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
  <env:Body>
    <test:echoIntegerArray xmlns:test="http://example.org/ts-tests"
          env:encodingStyle="http://www.w3.org/2003/05/soap-encoding">
      <inputIntegerArray enc:itemType="xsd:int" enc:arraySize="2"
                         xmlns:enc="http://www.w3.org/2003/05/soap-encoding">
        <item xsi:type="xsd:int">100</item>
        <item xsi:type="xsd:int">200</item>
      </inputIntegerArray>
    </test:echoIntegerArray>
  </env:Body>
</env:Envelope>
EOF;
include "soap12-test.inc";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope" xmlns:ns1="http://example.org/ts-tests" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:enc="http://www.w3.org/2003/05/soap-encoding" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns2="http://example.org/ts-tests/xsd"><env:Body xmlns:rpc="http://www.w3.org/2003/05/soap-rpc"><ns1:echoIntegerArrayResponse env:encodingStyle="http://www.w3.org/2003/05/soap-encoding"><rpc:result>return</rpc:result><return enc:itemType="xsd:int" enc:arraySize="2" xsi:type="ns2:ArrayOfint"><item xsi:type="xsd:int">100</item><item xsi:type="xsd:int">200</item></return></ns1:echoIntegerArrayResponse></env:Body></env:Envelope>
ok
