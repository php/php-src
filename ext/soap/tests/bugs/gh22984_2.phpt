--TEST--
GH-22984 (Native stack overflow while decoding a cyclic SOAP 1.2 enc:ref reference)
--EXTENSIONS--
soap
--FILE--
<?php
class TestSoapClient extends SoapClient {
    public function __doRequest($request, $location, $action, $version, $one_way = false): ?string {
        return <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope" xmlns:enc="http://www.w3.org/2003/05/soap-encoding" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:ns1="http://testuri.org">
  <env:Body>
    <ns1:probeResponse env:encodingStyle="http://www.w3.org/2003/05/soap-encoding">
      <ret enc:id="a" xsi:type="enc:Array" enc:itemType="enc:Array" enc:arraySize="1">
        <item enc:ref="#b" xsi:type="enc:Array" enc:itemType="enc:Array" enc:arraySize="1"/>
      </ret>
      <node enc:id="b" xsi:type="enc:Array" enc:itemType="enc:Array" enc:arraySize="1">
        <item enc:ref="#a" xsi:type="enc:Array" enc:itemType="enc:Array" enc:arraySize="1"/>
      </node>
    </ns1:probeResponse>
  </env:Body>
</env:Envelope>
XML;
    }
}

$client = new TestSoapClient(null, ['location' => 'test://', 'uri' => 'http://testuri.org', 'soap_version' => SOAP_1_2]);
var_dump($client->__soapCall('probe', []));
?>
--EXPECT--
array(2) {
  ["ret"]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(0) {
      }
    }
  }
  ["node"]=>
  array(1) {
    [0]=>
    array(0) {
    }
  }
}
