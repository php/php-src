--TEST--
SOAP typemap 2: SoapServer support for typemap's to_xml()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
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
	return '<book xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"><a xsi:type="xsd:string">'.$book->a.'!</a><b xsi:type="xsd:string">'.$book->b.'!</b></book>';
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

$server = new SoapServer(dirname(__FILE__)."/classmap.wsdl",$options);
$server->setClass("test");
$server->handle($HTTP_RAW_POST_DATA);
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://schemas.nothing.com" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:dotest2Response><book xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:type="ns1:book"><a xsi:type="xsd:string">foo!</a><b xsi:type="xsd:string">bar!</b></book></ns1:dotest2Response></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
