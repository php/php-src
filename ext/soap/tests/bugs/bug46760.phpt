--TEST--
Bug #46760 (SoapClient doRequest fails when proxy is used)
--EXTENSIONS--
soap
--FILE--
<?php

$client = new SoapClient(null, array('proxy_host'     => "localhost",
                                            'proxy_port'     => '8080',
                                            'login'    => "user",
                                            'password' => "test",
                                            'uri'            => 'mo:http://www.w3.org/',
                                            'location'       => 'http://some.url'));
var_dump($client);

?>
--EXPECTF--
%A
  ["_proxy_port":"SoapClient":private]=>
  int(8080)
%A
