--TEST--
array openssl_get_md_methods ([ bool $aliases = false ] );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--SKIPIF--
<?php 
if (!extension_loaded("openssl")) print "skip";
if (OPENSSL_VERSION_NUMBER < 0x10000000) die("skip Output requires OpenSSL 1.0");
?>
--FILE--
<?php
$getMdMethodsTrue = openssl_get_md_methods(true);
$getMdMethodsFalse = openssl_get_md_methods(false);

if ((is_array($getMdMethodsTrue)) && (!is_null($getMdMethodsTrue))) {
 print("okey");
} else {
 print("OpenSSL has failed to get md methods with the aliases argument set to true");
}

if ((is_array($getMdMethodsFalse)) && (!is_null($getMdMethodsFalse))) {
 print("okey");
} else {
 print("openssl has failure to get md method with false argument");
}
?>
--EXPECT--
okey
okey
