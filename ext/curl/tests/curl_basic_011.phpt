--TEST--
Test curl_opt() function with COOKIE
--CREDITS--
TestFest 2009 - AFUP - Xavier Gorse <xgorse@elao.com>
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
  include 'server.inc';
  $host = curl_cli_server_start();

  // start testing
  echo '*** Testing curl with cookie ***' . "\n";

  $url = "{$host}/get.inc?test=cookie";
  $ch = curl_init();

  ob_start(); // start output buffering
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
  curl_setopt($ch, CURLOPT_COOKIE, 'foo=bar');
  curl_setopt($ch, CURLOPT_URL, $url); //set the url we want to use

  $curl_content = curl_exec($ch);
  curl_close($ch);

  var_dump( $curl_content );
?>
--EXPECT--
*** Testing curl with cookie ***
string(3) "bar"
