--TEST--
GH-25628 (SOAP fails to import a schema with a DOCTYPE named schema, such as XMLDSIG)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$client = new SoapClient(__DIR__ . '/gh25628.wsdl');
var_dump($client->__getFunctions());
var_dump($client->__getTypes());
?>
--EXPECT--
array(1) {
  [0]=>
  string(39) "void Request(SignedRequest $parameters)"
}
array(2) {
  [0]=>
  string(39) "struct SignatureType {
 <anyXML> any;
}"
  [1]=>
  string(50) "struct SignedRequest {
 SignatureType Signature;
}"
}
