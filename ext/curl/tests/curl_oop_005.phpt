--TEST--
Test CurlHandle::setOpt() function with user agent
--CREDITS--
Based on curl_basic_005.phpt by:
Sebastian Deutsch <sebastian.deutsch@9elements.com>
TestFest 2009 - AFUP - Jean-Marc Fontaine <jmf@durcommefaire.net>
--EXTENSIONS--
curl
--FILE--
<?php
  include 'server.inc';
  $host = curl_cli_server_start();

  // start testing
  echo '*** Testing curl with user agent ***' . "\n";

  $url = "{$host}/get.inc?test=useragent";
  $curl_content = (new CurlHandle($url))
    ->setOpt(CURLOPT_USERAGENT, 'cURL phpt')
    ->exec();

  var_dump( $curl_content );
?>
--EXPECT--
*** Testing curl with user agent ***
string(9) "cURL phpt"
