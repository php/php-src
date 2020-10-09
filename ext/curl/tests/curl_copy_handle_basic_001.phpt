--TEST--
Test curl_copy_handle() with simple get
--CREDITS--
Rick Buitenman <rick@meritos.nl>
#testfest Utrecht 2009
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

  include 'server.inc';
  $host = curl_cli_server_start();

  echo '*** Testing curl copy handle with simple GET ***' . "\n";

  $url = "{$host}/get.inc?test=getpost&get_param=Hello%20World";
  $ch = curl_init();

  ob_start(); // start output buffering
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
  curl_setopt($ch, CURLOPT_URL, $url); //set the url we want to use

  $copy = curl_copy_handle($ch);
  curl_close($ch);

  $curl_content = curl_exec($copy);
  curl_close($copy);

  var_dump( $curl_content );
?>
--EXPECT--
*** Testing curl copy handle with simple GET ***
string(106) "array(2) {
  ["test"]=>
  string(7) "getpost"
  ["get_param"]=>
  string(11) "Hello World"
}
array(0) {
}
"
