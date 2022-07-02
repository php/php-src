--TEST--
Bug #41004 (minOccurs="0" and null class member variable)
--EXTENSIONS--
soap
--FILE--
<?php
ini_set('soap.wsdl_cache_enabled', false);

class EchoBean{
    public $mandatoryElement;
    public $optionalElement;

}

class EchoRequest{
    public $in;
}

class EchoResponse{
    public $out;
}

$wsdl = __DIR__."/bug41004.wsdl";
$classmap = array('EchoBean'=>'EchoBean','echo'=>'EchoRequest','echoResponse'=>'EchoResponse');
$client = new SoapClient($wsdl, array('location'=>'test://',"classmap" => $classmap, 'exceptions'=>0, 'trace'=>1));
$echo=new EchoRequest();
$in=new EchoBean();
$in->mandatoryElement="REV";
$in->optionalElement=NULL;
$echo->in=$in;
$client->echo($echo);
echo $client->__getLastRequest();
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="urn:Formation"><SOAP-ENV:Body><ns1:echo><in><mandatoryElement>REV</mandatoryElement></in></ns1:echo></SOAP-ENV:Body></SOAP-ENV:Envelope>
