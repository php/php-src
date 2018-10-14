--TEST--
Test curl_copy_handle() with simple POST
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
  include 'server.inc';
  $host = curl_cli_server_start();

  echo '*** Testing curl copy handle with simple POST using array as arguments ***' . "\n";

  $url = "{$host}/get.php?test=getpost";
  $ch = curl_init();

  ob_start(); // start output buffering
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
  curl_setopt($ch, CURLOPT_POST, 1);
  curl_setopt($ch, CURLOPT_POSTFIELDS, array("Hello" => "World", "Foo" => "Bar", "Person" => "John Doe"));
  curl_setopt($ch, CURLOPT_HTTPHEADER, array('Expect:')); // Disable Expect: header (lighttpd does not support it :)
  curl_setopt($ch, CURLOPT_URL, $url); //set the url we want to use

  $copy = curl_copy_handle($ch);
  curl_close($ch);

  $curl_content = curl_exec($copy);
  curl_close($copy);

  var_dump( $curl_content );
?>
===DONE===
--EXPECT--
*** Testing curl copy handle with simple POST using array as arguments ***
string(163) "array(1) {
  ["test"]=>
  string(7) "getpost"
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
