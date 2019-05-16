--TEST--
Test curl_copy_handle() with CURLOPT_PROGRESSFUNCTION
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
  include 'server.inc';
  $host = curl_cli_server_start();

  $url = "{$host}/get.inc";
  $ch = curl_init($url);

  curl_setopt($ch, CURLOPT_NOPROGRESS, 0);
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
  curl_setopt($ch, CURLOPT_PROGRESSFUNCTION, function() { });
  $ch2 = curl_copy_handle($ch);
  echo curl_exec($ch), PHP_EOL;
  unset($ch);
  echo curl_exec($ch2);

?>
--EXPECT--
Hello World!
Hello World!
Hello World!
Hello World!
