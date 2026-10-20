--TEST--
SoapClient::__setCookie with numeric keys
--EXTENSIONS--
soap
--FILE--
<?php
$client = new SoapClient(null, array('uri' => 'mo:http://www.w3.org/', 'location' => 'http://example.com'));
$client->__setCookie("123", "456");
var_dump($client->__getCookies());
$client->__setCookie("123", NULL);
var_dump($client->__getCookies());
?>
--EXPECT--
array(1) {
  [123]=>
  array(1) {
    [0]=>
    string(3) "456"
  }
}
array(0) {
}
