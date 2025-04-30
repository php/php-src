--TEST--
Test for bug #49898: SoapClient::__getCookies() implementation
--CREDITS--
Boro Sitnikovski <buritomath@yahoo.com>
--EXTENSIONS--
soap
--FILE--
<?php
$client = new SoapClient(null, array('uri' => 'mo:http://www.w3.org/', 'location' => 'http://some.url'));
$client->__setCookie("CookieTest", "HelloWorld");
var_dump($client->__getCookies()['CookieTest'][0]);
?>
--EXPECT--
string(10) "HelloWorld"
