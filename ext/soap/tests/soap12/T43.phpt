--TEST--
SOAP 1.2: T43 echoStructAsSimpleTypes
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
    <test:echoStructAsSimpleTypes xmlns:test="http://example.org/ts-tests"
          env:encodingStyle="http://www.w3.org/2003/05/soap-encoding">
      <inputStruct xsi:type="ns1:SOAPStruct"
                   xmlns:ns1="http://example.org/ts-tests/xsd">
        <varInt xsi:type="xsd:int">42</varInt>
        <varFloat xsi:type="xsd:float">0.005</varFloat>
        <varString xsi:type="xsd:string">hello world</varString>
      </inputStruct>
    </test:echoStructAsSimpleTypes>
  </env:Body>
</env:Envelope>
EOF;
include "soap12-test.inc";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope" xmlns:ns1="http://example.org/ts-tests" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:enc="http://www.w3.org/2003/05/soap-encoding"><env:Body><ns1:echoStructAsSimpleTypesResponse env:encodingStyle="http://www.w3.org/2003/05/soap-encoding"><outputString xsi:type="xsd:string">hello world</outputString><outputInteger xsi:type="xsd:int">42</outputInteger><outputFloat xsi:type="xsd:float">0.005</outputFloat></ns1:echoStructAsSimpleTypesResponse></env:Body></env:Envelope>
ok
