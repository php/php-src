--TEST--
Bug #41097 (ext/soap returning associative array as indexed without using WSDL)
--EXTENSIONS--
soap
--FILE--
<?php
function test($soap, $array) {
  $soap->test($array);
  echo (strpos($soap->__getLastRequest(), ':Map"') != false)?"Map\n":"Array\n";
}


$soap = new SoapClient(null, array('uri' => 'http://uri/', 'location' => 'test://', 'exceptions' => 0, 'trace' => 1));
test($soap, array('Foo', 'Bar'));
test($soap, array(5 => 'Foo', 10 => 'Bar'));
test($soap, array('5' => 'Foo', '10' => 'Bar'));
$soap->test(new SoapVar(array('Foo', 'Bar'), APACHE_MAP));
echo (strpos($soap->__getLastRequest(), ':Map"') != false)?"Map\n":"Array\n";
$soap->test(new SoapVar(array('Foo', 'Bar'), SOAP_ENC_ARRAY));
echo (strpos($soap->__getLastRequest(), ':Map"') != false)?"Map\n":"Array\n";
?>
--EXPECT--
Array
Map
Map
Map
Array
