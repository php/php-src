--TEST--
Test curl_opt() function with CURLOPT_HTTP_VERSION/CURL_HTTP_VERSION_1_0
--CREDITS--
TestFest 2009 - AFUP - Xavier Gorse <xgorse@elao.com>
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
  include 'server.inc';
  $host = curl_cli_server_start();

  // start testing
  echo '*** Testing curl with HTTP/1.0 ***' . "\n";

  $url = "{$host}/get.inc?test=httpversion";
  $ch = curl_init();

  ob_start(); // start output buffering
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
  curl_setopt($ch, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_0);
  curl_setopt($ch, CURLOPT_URL, $url); //set the url we want to use

  $curl_content = curl_exec($ch);
  curl_close($ch);

  var_dump( $curl_content );
?>
--EXPECT--
*** Testing curl with HTTP/1.0 ***
string(8) "HTTP/1.0"
                     
