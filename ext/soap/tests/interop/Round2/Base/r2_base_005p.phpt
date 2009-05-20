--TEST--
SOAP Interop Round2 base 005 (php/direct): echoString(utf-8)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php if (!function_exists('utf8_encode')) die('skip utf8_encode() not available'); ?>
--FILE--
<?php
$client = new SoapClient(NULL,array("location"=>"test://","uri"=>"http://soapinterop.org/","trace"=>1,"exceptions"=>0));
$client->__soapCall("echoString", array(utf8_encode('ỗÈéóÒ₧⅜ỗỸ')), array("soapaction"=>"http://soapinterop.org/","uri"=>"http://soapinterop.org/"));
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round2_base.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoString><param0 xsi:type="xsd:string">á»ÃÃ©Ã³Ãâ§âá»á»¸</param0></ns1:echoString></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:echoStringResponse><outputString xsi:type="xsd:string">á»ÃÃ©Ã³Ãâ§âá»á»¸</outputString></ns1:echoStringResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
