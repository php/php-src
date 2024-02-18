--TEST--
Test CurlHandle::setOpt() function with CURLOPT_RETURNTRANSFER parameter set to 0
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
  echo '*** Testing CurlHandle::setOpt(CURLOPT_RETURNTRANSFER, 0); ***' . "\n";

  $url = "{$host}/get.inc?test=get";
  $ch = new CurlHandle();

  ob_start();
  $ok = $ch
	->setOpt(CURLOPT_RETURNTRANSFER, 0)
	->setOpt(CURLOPT_URL, $url)
	->exec();
  $echoed_contents = ob_get_contents();
  ob_end_clean();

  var_dump($ok);
  var_dump($echoed_contents);
--EXPECT--
*** Testing CurlHandle::setOpt(CURLOPT_RETURNTRANSFER, 0); ***
bool(true)
string(25) "Hello World!
Hello World!"
