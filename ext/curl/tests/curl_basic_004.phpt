--TEST--
Test curl_opt() function with setting referer
--CREDITS--
Sebastian Deutsch <sebastian.deutsch@9elements.com>
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
  echo '*** Testing curl setting referer ***' . "\n";

  $url = "{$host}/get.php?test=referer";
  $ch = curl_init();

  ob_start(); // start output buffering
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
  curl_setopt($ch, CURLOPT_REFERER, 'http://www.refer.er');
  curl_setopt($ch, CURLOPT_URL, $url); //set the url we want to use
  
  $curl_content = curl_exec($ch);
  curl_close($ch);

  var_dump( $curl_content );
?>
===DONE===
--EXPECTF--
*** Testing curl setting referer ***
string(19) "http://www.refer.er"
===DONE===
