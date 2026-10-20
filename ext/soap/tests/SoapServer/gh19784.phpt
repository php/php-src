--TEST--
GH-19784 (SoapServer memory leak)
--EXTENSIONS--
soap
--FILE--
<?php

$options = ['encoding' => 'UTF-8'];
$server = new SoapServer('foobarbaz', $options);

?>
--EXPECTF--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>%s</faultcode><faultstring>%a
