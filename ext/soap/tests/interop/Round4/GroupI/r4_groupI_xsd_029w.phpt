--TEST--
SOAP Interop Round4 GroupI XSD 029 (php/wsdl): echoAnyElement
--SKIPIF--
<?php require_once('skipif.inc');  die('skip <any> is not supported yet'); ?>
--FILE--
<?php
$client = new SoapClient(dirname(__FILE__)."/round4_groupI_xsd.wsdl",array("trace"=>1,"exceptions"=>0));
$client->echoAnyElement(array("inputAny"=>(object)array("something"=>"Hello World")));
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round4_groupI_xsd.inc");
echo "ok\n";
?>
--EXPECT--
ok
