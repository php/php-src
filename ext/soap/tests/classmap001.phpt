--TEST--
SOAP Classmap 1: SoapServer support for classmap
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$GLOBALS['HTTP_RAW_POST_DATA']="
<env:Envelope xmlns:env=\"http://schemas.xmlsoap.org/soap/envelope/\"
    xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"
    xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"
    xmlns:enc=\"http://schemas.xmlsoap.org/soap/encoding/\"
    xmlns:ns1=\"http://schemas.nothing.com\"
>
  <env:Body>
 <dotest>
  <book xsi:type=\"ns1:book\">
    <a xsi:type=\"xsd:string\">Blaat</a>
    <b xsi:type=\"xsd:string\">aap</b>
</book>
</dotest>
 </env:Body>
<env:Header/>
</env:Envelope>";

class test{
    function dotest(book $book){
        $classname=get_class($book);
        return "Classname: ".$classname;
    }
}

class book{
    public $a="a";
    public $b="c";

}
$options=Array(
        'actor' =>'http://schema.nothing.com',
        'classmap' => array('book'=>'book', 'wsdltype2'=>'classname2')
        );

$server = new SoapServer(__DIR__."/classmap.wsdl",$options);
$server->setClass("test");
$server->handle($GLOBALS['HTTP_RAW_POST_DATA']);
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://schemas.nothing.com" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:dotestResponse><res xsi:type="xsd:string">Classname: book</res></ns1:dotestResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
