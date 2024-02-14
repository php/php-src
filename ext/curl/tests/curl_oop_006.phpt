--TEST--
Test CurlHandle::setOpt() function with CURLOPT_WRITEFUNCTION parameter set to a closure
--CREDITS--
Based on curl_basic_006.phpt by:
TestFest 2009 - AFUP - Jean-Marc Fontaine <jmf@durcommefaire.net>
--EXTENSIONS--
curl
--FILE--
<?php
  include 'server.inc';
  $host = curl_cli_server_start();

  // start testing
  echo '*** Testing $ch->setOpt(CURLOPT_WRITEFUNCTION, <closure>); ***' . "\n";

  $alldata = '';
  $url = "{$host}/get.inc?test=get";
  ob_start(); // start output buffering
  (new CurlHandle($url))->setOpt(CURLOPT_WRITEFUNCTION, function ($ch, $data) {
    $GLOBALS['alldata'] .= $data;
    return strlen ($data);
  })->exec();
  ob_end_flush();
  echo "Data: $alldata";
?>
===DONE===
--EXPECT--
*** Testing $ch->setOpt(CURLOPT_WRITEFUNCTION, <closure>); ***
Data: Hello World!
Hello World!===DONE===
