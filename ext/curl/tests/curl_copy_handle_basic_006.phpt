--TEST--
Test curl_copy_handle() with User Agent
--CREDITS--
Rick Buitenman <rick@meritos.nl>
#testfest Utrecht 2009
--SKIPIF--
<?php if (!extension_loaded("curl") || false === getenv('PHP_CURL_HTTP_REMOTE_SERVER')) print "skip need PHP_CURL_HTTP_REMOTE_SERVER environment variable"; ?>
--FILE--
<?php

  $host = getenv('PHP_CURL_HTTP_REMOTE_SERVER');

  echo '*** Testing curl copy handle with User Agent ***' . "\n";

  $url = "{$host}/get.php?test=useragent";
  $ch = curl_init();

  ob_start(); // start output buffering
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
  curl_setopt($ch, CURLOPT_USERAGENT, 'cURL phpt');
  curl_setopt($ch, CURLOPT_URL, $url); //set the url we want to use
  
  $copy = curl_copy_handle($ch);

  var_dump( curl_exec($ch) );
  var_dump( curl_exec($copy) );

  curl_close($ch); // can not close original handle before curl_exec($copy) since it causes char * inputs to be invalid (see also: http://curl.haxx.se/libcurl/c/curl_easy_duphandle.html)
  curl_close($copy);

?>
===DONE===
--EXPECTF--
*** Testing curl copy handle with User Agent ***
string(9) "cURL phpt"
string(9) "cURL phpt"
===DONE===  
