--TEST--
SOAP Typemap 3: SoapClient support for typemap's from_xml()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php if (!extension_loaded('simplexml')) die("skip simplexml extension not available"); ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
class TestSoapClient extends SoapClient{
  function __doRequest($request, $location, $action, $version, $one_way = 0) {
		return <<<EOF
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://schemas.nothing.com" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body>
<ns1:dotest2Response><res xsi:type="ns1:book">
  <a xsi:type="xsd:string">foo</a>
  <b xsi:type="xsd:string">bar</b>
</res>
</ns1:dotest2Response></SOAP-ENV:Body></SOAP-ENV:Envelope>
EOF;
	}	
}

class book{
	public $a="a";
	public $b="c";
		
}

function book_from_xml($xml) {
	$sxe = simplexml_load_string($xml);
	$obj = new book;
	$obj->a = (string)$sxe->a;
	$obj->b = (string)$sxe->b;
	return $obj;
}

$options=Array(
		'actor' =>'http://schemas.nothing.com',
		'typemap' => array(array("type_ns"   => "http://schemas.nothing.com",
		                         "type_name" => "book",
		                         "from_xml"  => "book_from_xml"))
		);

$client = new TestSoapClient(dirname(__FILE__)."/classmap.wsdl",$options);
$ret = $client->dotest2("???");
var_dump($ret);
echo "ok\n";
?>
--EXPECTF--
object(book)#%d (2) {
  ["a"]=>
  string(3) "foo"
  ["b"]=>
  string(3) "bar"
}
ok
