--TEST--
Test CurlHandle::setOpt() function with COOKIE
--CREDITS--
Based on curl_basic_011.phpt by:
TestFest 2009 - AFUP - Xavier Gorse <xgorse@elao.com>
--EXTENSIONS--
curl
--FILE--
<?php
  include 'server.inc';
  $host = curl_cli_server_start();

  // start testing
  echo '*** Testing curl with cookie ***' . "\n";

  $url = "{$host}/get.inc?test=cookie";
  $curl_content = (new CurlHandle($url))
    ->setOpt(CURLOPT_COOKIE, 'foo=bar')
    ->exec();

  var_dump( $curl_content );
?>
--EXPECT--
*** Testing curl with cookie ***
string(3) "bar"
