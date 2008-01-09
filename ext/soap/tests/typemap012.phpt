--TEST--
SOAP Typemap 12: SoapClient support for typemap's to_xml() (SoapFault)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
class TestSoapClient extends SoapClient{
  function __doRequest($request, $location, $action, $version) {
  		echo $request;
  		exit;
	}	
}

class book{
	public $a="a";
	public $b="c";
		
}

function book_to_xml($book) {
	throw new SoapFault("Client", "Conversion Error");
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
try {
	$ret = $client->dotest($book);
} catch (SoapFault $e) {
	$ret = "SoapFault = " . $e->faultcode . " - " . $e->faultstring;
}
var_dump($ret);
echo "ok\n";
?>
--EXPECT--
string(37) "SoapFault = Client - Conversion Error"
ok
