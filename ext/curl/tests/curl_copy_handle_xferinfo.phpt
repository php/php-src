--TEST--
Test curl_copy_handle() with CURLOPT_XFERINFOFUNCTION
--EXTENSIONS--
curl
--FILE--
<?php
  include 'server.inc';
  $host = curl_cli_server_start();

  $url = "{$host}/get.inc";
  $ch = curl_init($url);

  curl_setopt($ch, CURLOPT_NOPROGRESS, 0);
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
  curl_setopt($ch, CURLOPT_XFERINFOFUNCTION, function() { static $done = false; if (!$done) { echo "Download progress!\n"; $done = true; } });
  $ch2 = curl_copy_handle($ch);
  echo curl_exec($ch), PHP_EOL;
  unset($ch);
  echo curl_exec($ch2);

?>
--EXPECT--
Download progress!
Hello World!
Hello World!
Hello World!
Hello World!
