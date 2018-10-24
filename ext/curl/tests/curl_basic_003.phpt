--TEST--
Test curl_opt() function with POST parameters
--CREDITS--
Sebastian Deutsch <sebastian.deutsch@9elements.com>
TestFest 2009 - AFUP - Jean-Marc Fontaine <jmf@durcommefaire.net>
--SKIPIF--
<?php include 'skipif.inc'; ?>
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

  $url = "{$host}/get.inc?test=getpost&get_param=Hello%20World";
  $ch = curl_init();

  ob_start(); // start output buffering
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
  curl_setopt($ch, CURLOPT_POST, 1);
  curl_setopt($ch, CURLOPT_POSTFIELDS, "Hello=World&Foo=Bar&Person=John%20Doe");
  curl_setopt($ch, CURLOPT_URL, $url); //set the url we want to use

  $curl_content = curl_exec($ch);
  curl_close($ch);

  var_dump( $curl_content );
?>
===DONE===
--EXPECT--
*** Testing curl sending through GET an POST ***
string(208) "array(2) {
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
  ["Person"]=>
  string(8) "John Doe"
}
"
===DONE===
