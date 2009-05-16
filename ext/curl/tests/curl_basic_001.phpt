--TEST--
Test curl_exec() function with basic functionality 
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
/* Prototype  : bool curl_exec(resource ch)
 * Description: Perform a cURL session 
 * Source code: ext/curl/interface.c
 * Alias to functions: 
 */
	
  $host = getenv('PHP_CURL_HTTP_REMOTE_SERVER');

  // start testing
  echo "*** Testing curl_exec() : basic functionality ***\n";

  $url = "{$host}/get.php?test=get";
  $ch = curl_init();

  ob_start(); // start output buffering
  curl_setopt($ch, CURLOPT_URL, $url); //set the url we want to use
  $ok = curl_exec($ch);
  curl_close($ch);
  $curl_content = ob_get_contents();
  ob_end_clean();

  if($ok) {
    var_dump( $curl_content );
  } else {
    echo "curl_exec returned false";
  }
?>
===DONE===
--EXPECTF--
*** Testing curl_exec() : basic functionality ***
string(25) "Hello World!
Hello World!"
===DONE===
