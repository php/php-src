--TEST--
SOAP: SoapClient::__setHeaders
--EXTENSIONS--
soap
--FILE--
<?php
$client = new SoapClient(NULL, array("location"=>"test://","uri"=>"test://",
  "exceptions"=>0,"trace"=>1));
$client->test();
echo $client->__getLastRequest();
$client->__setSoapHeaders(new SoapHeader("test://","HDR1"));
$client->test();
echo $client->__getLastRequest();
$client->test();
echo $client->__getLastRequest();
$client->__setSoapHeaders();
$client->test();
echo $client->__getLastRequest();
$client->__setSoapHeaders(array(new SoapHeader("test://","HDR1"),new SoapHeader("test://","HDR2")));
$client->test();
echo $client->__getLastRequest();
$h = array(new SoapHeader("test://","HDR0"));
$client->__soapCall("test", array(), null, $h);
echo $client->__getLastRequest();
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="test://" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:test/></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="test://" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Header><ns1:HDR1/></SOAP-ENV:Header><SOAP-ENV:Body><ns1:test/></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="test://" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Header><ns1:HDR1/></SOAP-ENV:Header><SOAP-ENV:Body><ns1:test/></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="test://" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:test/></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="test://" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Header><ns1:HDR1/><ns1:HDR2/></SOAP-ENV:Header><SOAP-ENV:Body><ns1:test/></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="test://" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Header><ns1:HDR0/><ns1:HDR1/><ns1:HDR2/></SOAP-ENV:Header><SOAP-ENV:Body><ns1:test/></SOAP-ENV:Body></SOAP-ENV:Envelope>
