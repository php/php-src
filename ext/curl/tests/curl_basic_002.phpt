--TEST--
Test curl_opt() function with CURLOPT_RETURNTRANSFER parameter set to 1
--CREDITS--
Sebastian Deutsch <sebastian.deutsch@9elements.com>
TestFest 2009 - AFUP - Jean-Marc Fontaine <jmf@durcommefaire.net>
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
  include 'server.inc';
  $host = curl_cli_server_start();

  // start testing
  echo '*** Testing curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1); ***' . "\n";

  $url = "{$host}/get.inc?test=get";
  $ch = curl_init();

  ob_start(); // start output buffering
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
  curl_setopt($ch, CURLOPT_URL, $url); //set the url we want to use

  $curl_content = curl_exec($ch);
  curl_close($ch);

  var_dump( $curl_content );
?>
--EXPECT--
*** Testing curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1); ***
string(25) "Hello World!
Hello World!"
