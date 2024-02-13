--TEST--
Test CurlHandle::setOpt() function with CURLOPT_RETURNTRANSFER parameter set to 1
--CREDITS--
Based on curl_basic_002.phpt by:
Sebastian Deutsch <sebastian.deutsch@9elements.com>
TestFest 2009 - AFUP - Jean-Marc Fontaine <jmf@durcommefaire.net>
--EXTENSIONS--
curl
--FILE--
<?php
  include 'server.inc';
  $host = curl_cli_server_start();

  // start testing
  echo '*** Testing CurlHandle::setOpt(CURLOPT_RETURNTRANSFER, 1); ***' . "\n";

  $url = "{$host}/get.inc?test=get";
  $ch = new CurlHandle();

  $curl_content = $ch
	->setOpt(CURLOPT_RETURNTRANSFER, 1)
	->setOpt(CURLOPT_URL, $url)
	->exec();

  var_dump( $curl_content );
--EXPECT--
*** Testing CurlHandle::setOpt(CURLOPT_RETURNTRANSFER, 1); ***
string(25) "Hello World!
Hello World!"
