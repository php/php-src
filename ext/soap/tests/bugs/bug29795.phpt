--TEST--
Bug #29795 (SegFault with Soap and Amazon's Web Services)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
soap.wsdl_cache_enabled=1
--FILE--
<?php
class LocalSoapClient extends SoapClient {

  function __construct($wsdl, $options) {
    parent::__construct($wsdl, $options);
  }

  function __doRequest($request, $location, $action, $version, $one_way = 0) {
    return <<<EOF
<?xml version="1.0" encoding="UTF-8"?><SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/"
xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
xmlns:xsd="http://www.w3.org/2001/XMLSchema"><SOAP-ENV:Body><Price><Amount>3995</Amount><CurrencyCode>USD</CurrencyCode></Price></SOAP-ENV:Body></SOAP-ENV:Envelope>
EOF;
  }

}

$client = new LocalSoapClient(__DIR__."/bug29795.wsdl",array("trace"=>1));
$ar=$client->GetPrice();
echo "o";
$client = new LocalSoapClient(__DIR__."/bug29795.wsdl",array("trace"=>1));
$ar=$client->GetPrice();
echo "k\n";
?>
--EXPECT--
ok
