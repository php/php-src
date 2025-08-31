--TEST--
SOAP 1.2: T51 echoBase64
--EXTENSIONS--
soap
--FILE--
<?php
$HTTP_RAW_POST_DATA = <<<EOF
<?xml version="1.0"?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope"
              xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
              xmlns:xsd="http://www.w3.org/2001/XMLSchema">
  <env:Body>
    <test:echoBase64 xmlns:test="http://example.org/ts-tests"
       env:encodingStyle="http://www.w3.org/2003/05/soap-encoding">
      <inputBase64 xsi:type="xsd:base64Binary">
        YUdWc2JHOGdkMjl5YkdRPQ==
      </inputBase64>
    </test:echoBase64>
  </env:Body>
</env:Envelope>
EOF;
include "soap12-test.inc";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope" xmlns:ns1="http://example.org/ts-tests" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:enc="http://www.w3.org/2003/05/soap-encoding"><env:Body xmlns:rpc="http://www.w3.org/2003/05/soap-rpc"><ns1:echoBase64Response env:encodingStyle="http://www.w3.org/2003/05/soap-encoding"><rpc:result>return</rpc:result><return xsi:type="xsd:base64Binary">YUdWc2JHOGdkMjl5YkdRPQ==</return></ns1:echoBase64Response></env:Body></env:Envelope>
ok
