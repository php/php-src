--TEST--
Test CurlHandle::setOpt() method with POST parameters
--CREDITS--
Based on curl_basic_003.phpt by:
Sebastian Deutsch <sebastian.deutsch@9elements.com>
TestFest 2009 - AFUP - Jean-Marc Fontaine <jmf@durcommefaire.net>
--EXTENSIONS--
curl
--FILE--
<?php
  include 'server.inc';
  $host = curl_cli_server_start();

  // start testing
  echo '*** Testing curl sending through GET an POST ***' . "\n";

  $url = "{$host}/get.inc?test=getpost&get_param=Hello%20World";
  $curl_content = (new CurlHandle($url))
    ->setOpt(CURLOPT_POST, 1)
    ->setOpt(CURLOPT_POSTFIELDS, "Hello=World&Foo=Bar&Person=John%20Doe")
	->exec();

  var_dump( $curl_content );
?>
--EXPECT--
*** Testing curl sending through GET an POST ***
string(208) "array(2) {
  ["test"]=>
  string(7) "getpost"
  ["get_param"]=>
  string(11) "Hello World"
}
array(3) {
  ["Hello"]=>
  string(5) "World"
  ["Foo"]=>
  string(3) "Bar"
  ["Person"]=>
  string(8) "John Doe"
}
"
