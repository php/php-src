--TEST--
Test CurlHandle::getInfo() function with CURLINFO_EFFECTIVE_URL parameter
--CREDITS--
Jean-Marc Fontaine <jmf@durcommefaire.net>
--EXTENSIONS--
curl
--FILE--
<?php
  include 'server.inc';
  $host = curl_cli_server_start();

  $url = "http://{$host}/get.inc?test=";
  $ch  = (new CurlHandle($url))->setOpt(CURLOPT_RETURNTRANSFER, 0);
  $ch->exec();

  $info = $ch->getInfo(CURLINFO_EFFECTIVE_URL);
  echo PHP_EOL;
  var_dump($url == $info);
--EXPECT--
Hello World!
Hello World!
bool(true)
