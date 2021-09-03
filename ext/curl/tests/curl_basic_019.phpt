--TEST--
Test curl_getinfo() function with CURLINFO_EFFECTIVE_URL parameter
--CREDITS--
Jean-Marc Fontaine <jmf@durcommefaire.net>
--EXTENSIONS--
curl
--FILE--
<?php
  include 'server.inc';
  $host = curl_cli_server_start();

  $url = "http://{$host}/get.inc?test=";
  $ch  = curl_init();

  curl_setopt($ch, CURLOPT_URL, $url);
  curl_exec($ch);
  $info = curl_getinfo($ch, CURLINFO_EFFECTIVE_URL);
  var_dump($url == $info);
  curl_close($ch);
?>
--EXPECT--
Hello World!
Hello World!bool(true)
