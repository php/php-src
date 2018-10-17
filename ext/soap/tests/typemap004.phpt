--TEST--
SOAP Typemap 4: SoapClient support for typemap's to_xml()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
class TestSoapClient extends SoapClient{
  function __doRequest($request, $location, $action, $version, $one_way = 0) {
  		echo $request;
  		exit;
	}
}

class book{
	public $a="a";
	public $b="c";

}

function book_to_xml($book) {
	return '<book xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"><a xsi:type="xsd:string">'.$book->a.'!</a><b xsi:type="xsd:string">'.$book->b.'!</b></book>';
}

$options=Array(
		'actor' =>'http://schemas.nothing.com',
		'typemap' => array(array("type_ns"   => "http://schemas.nothing.com",
		                         "type_name" => "book",
		                         "to_xml"  => "book_to_xml"))
		);

$client = new TestSoapClient(dirname(__FILE__)."/classmap.wsdl",$options);
$book = new book();
$book->a = "foo";
$book->b = "bar";
$ret = $client->dotest($book);
var_dump($ret);
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://schemas.nothing.com" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:dotest><book xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:type="ns1:book"><a xsi:type="xsd:string">foo!</a><b xsi:type="xsd:string">bar!</b></book></ns1:dotest></SOAP-ENV:Body></SOAP-ENV:Envelope>
