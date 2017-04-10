--TEST--
Bug #74402 (segfault on random_bytes, bin3hex, openssl_seal)
--SKIPIF--
<?php
if (!extension_loaded("openssl")) {
	print "skip";
}
if (!in_array('AES256', openssl_get_cipher_methods(true))) {
	print "skip";
}
?>
--FILE--
<?php
$data = "23153b1cf683cb16f8d71190a7c42f38fecda27c29a7bc8991c9f6a2a63602bf";
$key = array("-----BEGIN PUBLIC KEY-----
MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAqvjCLfpS0MyilIjR+IsH
HPH8TqFUCw4kTAVmTy9SDZV9hHYY2EPgrlTd7gvMP/DWipvBD6Y5w2bPdAQoXr5D
qEKAGkE+1El4hS8XyuOdYXSYTDH1HPSlFiGdgsnlkFcbh/fJyzIKBaGLnWxsjhiS
deiI7KuEkI9zt+X2r4KqFt/dhnXz0kcB1M7qyhQ6Rvijgjy/A1LsN4ZAREFLCEjb
1AP9nk0QAUHWcG5MvbgsE20Pn4R5wFsMFBTvNmb34jHFREgR9j4DYcV5FFR3tKb8
3XtjE9/kjfK29BSpiyXZs8PSqDhO00vh6txUB4VfkVUD2Bi93rxDeyALnCW7My+l
YwIDAQAB
-----END PUBLIC KEY-----");
$iv = '';
var_dump(strlen($data));
var_dump(openssl_seal($data, $sealed_data, $env_keys, $key, 'AES256', $iv));
?>
--EXPECTF--
int(64)
int(80)
