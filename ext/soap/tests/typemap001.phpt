--TEST--
SOAP typemap 1: SoapServer support for typemap's from_xml()
--EXTENSIONS--
soap
simplexml
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
 <ns1:dotest>
  <book xsi:type=\"ns1:book\">
    <a xsi:type=\"xsd:string\">foo</a>
    <b xsi:type=\"xsd:string\">bar</b>
</book>
</ns1:dotest>
 </env:Body>
<env:Header/>
</env:Envelope>";

function book_from_xml($xml) {
    $sxe = simplexml_load_string($xml);
    $obj = new book;
    $obj->a = (string)$sxe->a;
    $obj->b = (string)$sxe->b;
    return $obj;
}

class test{
    function dotest($book){
        $classname=get_class($book);
        return "Object: ".$classname. "(".$book->a.",".$book->b.")";
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
                                 "from_xml"  => "book_from_xml"))
        );

$server = new SoapServer(__DIR__."/classmap.wsdl",$options);
$server->setClass("test");
$server->handle($HTTP_RAW_POST_DATA);
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://schemas.nothing.com" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:dotestResponse><res xsi:type="xsd:string">Object: book(foo,bar)</res></ns1:dotestResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
