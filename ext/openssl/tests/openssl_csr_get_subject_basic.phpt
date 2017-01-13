--TEST--
openssl_csr_get_subject() tests
--SKIPIF--
<?php 
if (!extension_loaded("openssl")) die("skip"); 
if (!function_exists("utf8_decode")) die("skip");
?>
--FILE--
<?php

$csr = file_get_contents(dirname(__FILE__) . '/cert.csr');
if ($out = openssl_csr_get_subject($csr, 1)) {
	var_dump($out);
}
echo "\n";
$cn = utf8_decode($out['CN']);
var_dump($cn);
--EXPECTF--	
array(6) {
  ["C"]=>
  string(2) "NL"
  ["ST"]=>
  string(13) "Noord Brabant"
  ["L"]=>
  string(4) "Uden"
  ["O"]=>
  string(10) "Triconnect"
  ["OU"]=>
  string(10) "Triconnect"
  ["CN"]=>
  string(15) "*.triconnect.nl"
}

string(15) "*.triconnect.nl"
