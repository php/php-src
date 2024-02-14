--TEST--
Test CurlHandle::setOpt() function with setting referer
--CREDITS--
Based on curl_basic_004.phpt by:
Sebastian Deutsch <sebastian.deutsch@9elements.com>
TestFest 2009 - AFUP - Jean-Marc Fontaine <jmf@durcommefaire.net>
--EXTENSIONS--
curl
--FILE--
<?php
  include 'server.inc';
  $host = curl_cli_server_start();

  // start testing
  echo '*** Testing curl setting referer ***' . "\n";

  $url = "{$host}/get.inc?test=referer";
  $curl_content = (new CurlHandle($url))
    ->setOpt(CURLOPT_REFERER, 'http://www.refer.er')
    ->exec();

  var_dump( $curl_content );
?>
--EXPECT--
*** Testing curl setting referer ***
string(19) "http://www.refer.er"
