--TEST--
SOAP 1.2: T40 echoOK
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$HTTP_RAW_POST_DATA = <<<EOF
<?xml version='1.0' ?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope"> 
  <env:Header>
    <test:Unknown xmlns:test="http://[FEDC:BA98:7654:3210:FEDC:BA98:7654:3210]/ts-tests"
          env:role="http://www.w3.org/2003/05/soap-envelope/role/ultimateReceiver" 
          env:mustUnderstand="false">
      foo
    </test:Unknown>
  </env:Header>
  <env:Body>
  </env:Body>
</env:Envelope>
EOF;
include "soap12-test.inc";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope"><env:Body/></env:Envelope>
ok
