--TEST--
Test CurlHandle::getInfo() function with CURLINFO_HTTP_CODE parameter
--CREDITS--
Based on curl_basic_020.phpt by:
Jean-Marc Fontaine <jmf@durcommefaire.net>
--EXTENSIONS--
curl
--FILE--
<?php
  include 'server.inc';
  $host = curl_cli_server_start();

  $url = "{$host}/get.inc?test=";
  $ch  = (new CurlHandle($url))->setOpt(CURLOPT_RETURNTRANSFER, 0);
  $ok = $ch->exec();
  echo PHP_EOL;
  var_dump($ok);
  var_dump($ch->getInfo(CURLINFO_HTTP_CODE));
?>
--EXPECT--
Hello World!
Hello World!
bool(true)
int(200)
