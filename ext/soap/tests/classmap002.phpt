--TEST--
SOAP Classmap 2: SoapClient support for classmap
--SKIPIF--
<?php require_once('skipif.inc'); ?>
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
  <a xsi:type="xsd:string">Blaat</a>
  <b xsi:type="xsd:string">aap</b>
</res>
</ns1:dotest2Response></SOAP-ENV:Body></SOAP-ENV:Envelope>
EOF;
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

$client = new TestSoapClient(dirname(__FILE__)."/classmap.wsdl",$options);
$ret = $client->dotest2("???");
var_dump($ret);
echo "ok\n";
?>
--EXPECT--
object(book)#2 (2) {
  ["a"]=>
  string(5) "Blaat"
  ["b"]=>
  string(3) "aap"
}
ok
