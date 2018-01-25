--TEST--
Bug #30106 (SOAP cannot not parse 'ref' element. Causes Uncaught SoapFault exception)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
ini_set("soap.wsdl_cache_enabled", 0);

function getContinentList() {
	return array("getContinentListResult"=>array(
	  "schema"=>"<xsd:schema><element name=\"test\" type=\"xsd:string\"/></xsd:schema>",
	  "any"=>"<test>Hello World!</test><test>Bye World!</test>"));
}

class LocalSoapClient extends SoapClient {
  function __construct($wsdl, $options=array()) {
    parent::__construct($wsdl, $options);
    $this->server = new SoapServer($wsdl, $options);
		$this->server->addFunction("getContinentList");
  }

  function __doRequest($request, $location, $action, $version, $one_way = 0) {
  	echo $request;
    ob_start();
    $this->server->handle($request);
    $response = ob_get_contents();
    ob_end_clean();
  	echo $response;
    return $response;
  }
}

$client = new LocalSoapClient(dirname(__FILE__)."/bug30106.wsdl");
var_dump($client->__getFunctions());
var_dump($client->__getTypes());
$x = $client->getContinentList(array("AFFILIATE_ID"=>1,"PASSWORD"=>"x"));
var_dump($x);
?>
--EXPECTF--
array(1) {
  [0]=>
  string(71) "getContinentListResponse getContinentList(getContinentList $parameters)"
}
array(3) {
  [0]=>
  string(64) "struct getContinentList {
 int AFFILIATE_ID;
 string PASSWORD;
}"
  [1]=>
  string(83) "struct getContinentListResponse {
 getContinentListResult getContinentListResult;
}"
  [2]=>
  string(66) "struct getContinentListResult {
 <anyXML> schema;
 <anyXML> any;
}"
}
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://tempuri.org/PRWebServ/getOtherInformation"><SOAP-ENV:Body><ns1:getContinentList><ns1:AFFILIATE_ID>1</ns1:AFFILIATE_ID><ns1:PASSWORD>x</ns1:PASSWORD></ns1:getContinentList></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:ns1="http://tempuri.org/PRWebServ/getOtherInformation"><SOAP-ENV:Body><ns1:getContinentListResponse><ns1:getContinentListResult><xsd:schema><element name="test" type="xsd:string"/></xsd:schema><test>Hello World!</test><test>Bye World!</test></ns1:getContinentListResult></ns1:getContinentListResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
object(stdClass)#%d (1) {
  ["getContinentListResult"]=>
  object(stdClass)#%d (2) {
    ["schema"]=>
    string(65) "<xsd:schema><element name="test" type="xsd:string"/></xsd:schema>"
    ["any"]=>
    string(48) "<test>Hello World!</test><test>Bye World!</test>"
  }
}
