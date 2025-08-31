--TEST--
SOAP typemap 10: SoapServer support for typemap's to_xml() (SoapFault)
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
<ns1:dotest2>
<dotest2 xsi:type=\"xsd:string\">???</dotest2>
</ns1:dotest2>
 </env:Body>
<env:Header/>
</env:Envelope>";

function book_to_xml($book) {
    throw new SoapFault("Server", "Conversion Fault");
}

class test{
    function dotest2($str){
        $book = new book;
        $book->a = "foo";
        $book->b = "bar";
        return $book;
    }
}

class book{
    public $a="a";
    public $b="c";

}

$options=Array(
        'actor'   =>'http://schemas.nothing.com',
        'typemap' => array(array("type_ns"   => "http://schemas.nothing.com",
                                 "type_name" => "book",
                                 "to_xml"    => "book_to_xml"))
        );

$server = new SoapServer(__DIR__."/classmap.wsdl",$options);
$server->setClass("test");
$server->handle($HTTP_RAW_POST_DATA);
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>Conversion Fault</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
