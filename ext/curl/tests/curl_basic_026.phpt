--TEST--
Test curl_seopt with CURLOPT_CURLU
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] < 0x073f00) die('skip requires curl >= 7.63.0');
?>
--FILE--
<?php
  include 'server.inc';
  $host = curl_cli_server_start();

  $url = "http://{$host}/get.inc?test=";
  $ch  = curl_init();
  curl_setopt($ch, CURLOPT_CURLU, new CurlUrl($url));
  curl_exec($ch);
  var_dump(curl_getinfo($ch, CURLINFO_HTTP_CODE));
  curl_close($ch);
?>
--EXPECT--
Hello World!
Hello World!int(200)
