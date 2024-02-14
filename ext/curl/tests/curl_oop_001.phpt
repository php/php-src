--TEST--
Test CurlHandle::exec() function with basic functionality
--CREDITS--
Based on curl_basic_001.phpt by:
Sebastian Deutsch <sebastian.deutsch@9elements.com>
TestFest 2009 - AFUP - Jean-Marc Fontaine <jmf@durcommefaire.net>
--EXTENSIONS--
curl
--FILE--
<?php
  include 'server.inc';
  $host = curl_cli_server_start();

  // start testing
  echo "*** Testing CurlHandle::exec() : basic functionality ***\n";

  $url = "{$host}/get.inc?test=get";
  $ch = new CurlHandle();

  ob_start(); // start output buffering
  $returned_content = $ch->setOpt(CURLOPT_URL, $url)->exec();
  unset($ch);
  $echoed_content = ob_get_contents();
  ob_end_clean();

  var_dump($echoed_content);
  var_dump($returned_content);
?>
--EXPECT--
*** Testing CurlHandle::exec() : basic functionality ***
string(0) ""
string(25) "Hello World!
Hello World!"
