--TEST--
GH-12392 (Segmentation fault on SoapClient::__getTypes)
--EXTENSIONS--
soap
--FILE--
<?php

$client = new SoapClient(__DIR__ . "/gh12392.wsdl", ['cache_wsdl' => WSDL_CACHE_NONE]);
echo 'Client created!' . "\n";

$types = $client->__getTypes();
echo 'Got types!' . "\n";

var_dump($types);

?>
--EXPECT--
Client created!
Got types!
array(1) {
  [0]=>
  string(62) "struct dummy {
 string foo;
 string a;
 string b;
 string c;
}"
}
