--TEST--
Bug #66112 (Use after free condition in SOAP extension)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
define('WSDL', dirname(__FILE__)."/bug66112.wsdl");
function Mist($p) {
	$client=new soapclient(WSDL, array('typemap'=>array(array("type_ns"=>"uri:mist", "type_name"=>"A"))));
	try{
		$client->Mist(array("XX"=>"xx"));
	}catch(SoapFault $x){
	}
	return array("A"=>"ABC","B"=>"sss");
}
$s = new SoapServer(WSDL, array('typemap'=>array(array("type_ns"=>"uri:mist", "type_name"=>"A"))));
$s->addFunction("Mist");
$_SERVER["REQUEST_METHOD"] = "POST";
$HTTP_RAW_POST_DATA=<<<EOF
<?xml version="1.0" encoding="UTF-8"?>
<soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:uri="uri:mist">
   <soapenv:Header/>
   <soapenv:Body>
      <uri:Request><uri:A>XXX</uri:A><uri:B>yyy</uri:B></uri:Request>
   </soapenv:Body>
</soapenv:Envelope>
EOF;
$s->handle($HTTP_RAW_POST_DATA);
echo "OK\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="uri:mist"><SOAP-ENV:Body><ns1:Response><A>ABC</A><B>sss</B></ns1:Response></SOAP-ENV:Body></SOAP-ENV:Envelope>
OK
