--TEST--
Test curl_opt() function with CURLOPT_WRITEFUNCTION parameter set to a closure
--SKIPIF--
<?php if (!extension_loaded("curl") || false === getenv(b'PHP_CURL_HTTP_REMOTE_SERVER')) print "skip"; ?>
--FILE--
<?php
/* Prototype  : bool curl_setopt(resource ch, int option, mixed value)
 * Description: Set an option for a cURL transfer
 * Source code: ext/curl/interface.c
 * Alias to functions:
 */

  $host = getenv('PHP_CURL_HTTP_REMOTE_SERVER');

  // start testing
  echo '*** Testing curl_setopt($ch, CURLOPT_WRITEFUNCTION, <closure>); ***' . "\n";

  $url = "{$host}/get.php?test=get";
  $ch = curl_init();

  ob_start(); // start output buffering
  curl_setopt($ch, CURLOPT_URL, $url); //set the url we want to use
  curl_setopt($ch, CURLOPT_WRITEFUNCTION, function ($ch, $data) {
    echo 'Data: '.$data;
    return strlen ($data);
  });
  
  curl_exec($ch);
  curl_close($ch);
?>
===DONE===
--EXPECTF--
*** Testing curl_setopt($ch, CURLOPT_WRITEFUNCTION, <closure>); ***
Data: Hello World!
Hello World!===DONE===
