--TEST--
Test CurlHandle:getInfo() function with CURLINFO_CONTENT_TYPE parameter
--CREDITS--
Based on curl_basic_021.phpt by:
Jean-Marc Fontaine <jmf@durcommefaire.net>
--EXTENSIONS--
curl
--FILE--
<?php
  include 'server.inc';
  $host = curl_cli_server_start();
  $url  = "{$host}/get.inc?test=contenttype";

  $ch = (new CurlHandle($url))->setOpt(CURLOPT_RETURNTRANSFER, 0);
  $ok = $ch->exec();
  var_dump($ok);
  var_dump(curl_getinfo($ch, CURLINFO_CONTENT_TYPE));
  curl_close($ch);
?>
--EXPECT--
bool(true)
string(24) "text/plain;charset=utf-8"
