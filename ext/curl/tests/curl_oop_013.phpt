--TEST--
Test CurlHandle::setOpt() function with CURLOPT_HTTP_VERSION/CURL_HTTP_VERSION_1_1
--CREDITS--
Based on curl_basic_013.phpt by:
TestFest 2009 - AFUP - Xavier Gorse <xgorse@elao.com>
--EXTENSIONS--
curl
--FILE--
<?php
  include 'server.inc';
  $host = curl_cli_server_start();

  // start testing
  echo '*** Testing curl with HTTP/1.1 ***' . "\n";

  $url = "{$host}/get.inc?test=httpversion";
  $curl_content = (new CurlHandle($url))
    ->setOpt(CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1)
    ->exec();

  var_dump( $curl_content );
?>
--EXPECT--
*** Testing curl with HTTP/1.1 ***
string(8) "HTTP/1.1"
                     
