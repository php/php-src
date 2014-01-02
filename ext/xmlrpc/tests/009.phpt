--TEST--
xmlrpc_decode() 64bit integer decode type i8

--SKIPIF--
<?php
if (!extension_loaded("xmlrpc")) die("skip");
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>

--FILE--
<?php

$xml = <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<methodResponse>
<params>
<param><value><i8>8182349955</i8></value></param>
</params>
</methodResponse>
XML;

$response = xmlrpc_decode($xml);
echo $response;

--EXPECT--
8182349955
