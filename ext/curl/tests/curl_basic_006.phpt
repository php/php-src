--TEST--
Test curl_opt() function with CURLOPT_WRITEFUNCTION parameter set to a closure
--CREDITS--
?
TestFest 2009 - AFUP - Jean-Marc Fontaine <jmf@durcommefaire.net>
--SKIPIF--
<?php 
if (!extension_loaded("curl")) exit("skip curl extension not loaded");
if (false === getenv('PHP_CURL_HTTP_REMOTE_SERVER'))  exit("skip PHP_CURL_HTTP_REMOTE_SERVER env variable is not defined");
?>
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
