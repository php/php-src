--TEST--
openssl_get_cert_locations() tests
--EXTENSIONS--
openssl
--FILE--
<?php
// openssl locations differ per distro.
var_dump(openssl_get_cert_locations());
?>
--EXPECTF--
array(8) {
  ["default_cert_file"]=>
  string(%d) "%s"
  ["default_cert_file_env"]=>
  string(%d) "%s"
  ["default_cert_dir"]=>
  string(%d) "%s"
  ["default_cert_dir_env"]=>
  string(%d) "%s"
  ["default_private_dir"]=>
  string(%d) "%s"
  ["default_default_cert_area"]=>
  string(%d) "%s"
  ["ini_cafile"]=>
  string(%d) ""
  ["ini_capath"]=>
  string(%d) ""
}
