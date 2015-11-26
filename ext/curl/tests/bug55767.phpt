--TEST--
Test curl_opt() function with POST params from array with a numeric key
--SKIPIF--
<?php 
include 'skipinf.inc';
?>
--FILE--
<?php
/* Prototype  : bool curl_setopt(resource ch, int option, mixed value)
 * Description: Set an option for a cURL transfer
 * Source code: ext/curl/interface.c
 * Alias to functions:
 */

  include 'server.inc';
  $host = curl_cli_server_start();

  // start testing
  echo '*** Testing curl sending through GET an POST ***' . "\n";

  $url = "{$host}/get.php?test=getpost&get_param=Hello%20World";
  $ch = curl_init();

  ob_start(); // start output buffering
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
  curl_setopt($ch, CURLOPT_POST, 1);
  curl_setopt($ch, CURLOPT_POSTFIELDS, array('Hello'=>'World','Foo'=>'Bar',100=>'John Doe'));
  curl_setopt($ch, CURLOPT_URL, $url); //set the url we want to use
  
  $curl_content = curl_exec($ch);
  curl_close($ch);

  var_dump( $curl_content );
?>
===DONE===
--EXPECTF--
*** Testing curl sending through GET an POST ***
string(203) "array(2) {
  ["test"]=>
  string(7) "getpost"
  ["get_param"]=>
  string(11) "Hello World"
}
array(3) {
  ["Hello"]=>
  string(5) "World"
  ["Foo"]=>
  string(3) "Bar"
  [100]=>
  string(8) "John Doe"
}
"
===DONE===
