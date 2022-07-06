--TEST--
SOAP Interop Round3 GroupD Doc Lit Parameters 003 (php/wsdl): echoStruct
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
precision=14
soap.wsdl_cache_enabled=0
--FILE--
<?php
class SOAPStruct {
    function __construct($s, $i, $f) {
        $this->varString = $s;
        $this->varInt = $i;
        $this->varFloat = $f;
    }
}
$struct = new SOAPStruct('arg',34,325.325);
$client = new SoapClient(__DIR__."/round3_groupD_doclitparams.wsdl",array("trace"=>1,"exceptions"=>0));
$client->echoStruct(array("param0"=>$struct));
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round3_groupD_doclitparams.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/xsd"><SOAP-ENV:Body><ns1:echoStruct><ns1:param0><ns1:varFloat>325.325</ns1:varFloat><ns1:varInt>34</ns1:varInt><ns1:varString>arg</ns1:varString></ns1:param0></ns1:echoStruct></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/xsd"><SOAP-ENV:Body><ns1:echoStructResponse><ns1:return><ns1:varFloat>325.325</ns1:varFloat><ns1:varInt>34</ns1:varInt><ns1:varString>arg</ns1:varString></ns1:return></ns1:echoStructResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
