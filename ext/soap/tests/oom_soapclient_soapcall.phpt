--TEST--
SoapClient::__soapCall() doesn't reach hard limit
--EXTENSIONS--
soap
--INI--
memory_limit=64M
--FILE--
<?php
require __DIR__ . '/oom_soap_response.inc';

$client = new OomSoapClient(null, ['location' => 'http://test-uri/', 'uri' => 'http://test-uri/']);
$client->response = oom_soap_response(1000, 1000);
$client->__soapCall('f', []);
?>
--EXPECTF--
Fatal error: Allowed memory size of 67108864 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
