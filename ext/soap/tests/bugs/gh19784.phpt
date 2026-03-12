--TEST--
GH-19784 (SoapServer memory leak)
--EXTENSIONS--
soap
--FILE--
<?php
$v_5256 = 'zpHOks6TzpTOlc6WzpfOmM6ZzprOm86czp3Ons6fzqDOoc6jzqTOpc6mzqfOqM6p';
$v_5257 = base64_decode($v_5256,);
$v_5238 = array('encoding' => $v_5257);
new SoapServer('foobarbaz',$v_5238,);
?>
--EXPECTF--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>%s</faultcode><faultstring>%a
