--TEST--
Bug #36908 (wsdl default value overrides value in soap request)
--SKIPIF--
<?php
  if (!extension_loaded('soap')) die('skip soap extension not available');
?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
class PublisherService {
  function add($publisher) {
    return $publisher->region_id;
  }
}
$input =
'<?xml version="1.0" encoding="UTF-8"?>
<soapenv:Envelope
xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/"
xmlns:xsd="http://www.w3.org/2001/XMLSchema"
xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
  <soapenv:Body>
    <ns1:add xmlns:ns1="urn:PublisherService" soapenv:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
      <publisher href="#id0"/>
    </ns1:add>
    <multiRef xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/"
xmlns:ns3="http://soap.dev/soap/types" id="id0" soapenc:root="0"
soapenv:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"
xsi:type="ns3:publisher">
      <region_id href="#id5"/>
    </multiRef>
    <multiRef xmlns:ns5="http://soap.dev/soap/types"
xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/" id="id5"
soapenc:root="0"
soapenv:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"
xsi:type="xsd:long">9</multiRef>
  </soapenv:Body>
</soapenv:Envelope>';
ini_set('soap.wsdl_cache_enabled', false);
$server = new SoapServer(dirname(__FILE__)."/bug36908.wsdl");
$server->setClass("PublisherService");
$server->handle($input);
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="urn:PublisherService" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:addResponse><out xsi:type="xsd:string">9</out></ns1:addResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
