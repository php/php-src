--TEST--
SOAP 1.2: T47 echoFloatArray
--EXTENSIONS--
soap
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
    <test:echoFloatArray xmlns:test="http://example.org/ts-tests"
          env:encodingStyle="http://www.w3.org/2003/05/soap-encoding">
      <inputFloatArray enc:itemType="xsd:float" enc:arraySize="2"
                       xmlns:enc="http://www.w3.org/2003/05/soap-encoding">
        <item xsi:type="xsd:float">5.5</item>
        <item xsi:type="xsd:float">12999.9</item>
      </inputFloatArray>
    </test:echoFloatArray>
  </env:Body>
</env:Envelope>
EOF;
include "soap12-test.inc";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope" xmlns:ns1="http://example.org/ts-tests" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:enc="http://www.w3.org/2003/05/soap-encoding" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ns2="http://example.org/ts-tests/xsd"><env:Body xmlns:rpc="http://www.w3.org/2003/05/soap-rpc"><ns1:echoFloatArrayResponse env:encodingStyle="http://www.w3.org/2003/05/soap-encoding"><rpc:result>return</rpc:result><return enc:itemType="xsd:float" enc:arraySize="2" xsi:type="ns2:ArrayOffloat"><item xsi:type="xsd:float">5.5</item><item xsi:type="xsd:float">12999.9</item></return></ns1:echoFloatArrayResponse></env:Body></env:Envelope>
ok
