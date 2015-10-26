--TEST--
array openssl_get_md_methods ([ bool $aliases = false ] );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--SKIPIF--
<?php 
if (!extension_loaded("openssl")) print "skip";
?>
--FILE--
<?php
$get_md_methods_true = openssl_get_md_methods(true);
$get_md_methods_false = openssl_get_md_methods(false);

if (is_array($get_md_methods_true)) {
 print("okey");
} else {
 print("OpenSSL has failed to get md methods with the aliases argument set to true");
}

if (is_array($get_md_methods_false)) {
 print("okey");
} else {
 print("OpenSSL has failed to get md methods with the aliases argument set to false");
}
?>
--EXPECT--
okey
okey
