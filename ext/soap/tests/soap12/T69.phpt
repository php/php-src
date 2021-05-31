--TEST--
SOAP 1.2: T69 echoOk
--EXTENSIONS--
soap
--FILE--
<?php
$HTTP_RAW_POST_DATA = <<<EOF
<?xml version='1.0' ?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope">
 <env:Header>
    <test:echoOk xmlns:test="http://example.org/ts-tests">foo</test:echoOk>
 </env:Header>
</env:Envelope>
EOF;
include "soap12-test.inc";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope"><env:Body><env:Fault><env:Code><env:Value>env:Sender</env:Value></env:Code><env:Reason><env:Text>Body must be present in a SOAP envelope</env:Text></env:Reason></env:Fault></env:Body></env:Envelope>
