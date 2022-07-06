--TEST--
openssl_pbkdf2() tests
--SKIPIF--
<?php if (!extension_loaded("openssl") || !function_exists("openssl_pbkdf2")) print "skip"; ?>
--FILE--
<?php
// official test vectors
var_dump(bin2hex(openssl_pbkdf2('password', 'salt', 20, 1)));
var_dump(bin2hex(openssl_pbkdf2('password', 'salt', 20, 2)));
var_dump(bin2hex(openssl_pbkdf2('password', 'salt', 20, 4096)));

/* really slow but should be:
string(40) "eefe3d61cd4da4e4e9945b3d6ba2158c2634e984"
var_dump(bin2hex(openssl_pbkdf2('password', 'salt', 20, 16777216)));
*/

var_dump(bin2hex(openssl_pbkdf2('passwordPASSWORDpassword', 'saltSALTsaltSALTsaltSALTsaltSALTsalt', 25, 4096)));
var_dump(bin2hex(openssl_pbkdf2("pass\0word", "sa\0lt", 16, 4096)));

?>
--EXPECT--
string(40) "0c60c80f961f0e71f3a9b524af6012062fe037a6"
string(40) "ea6c014dc72d6f8ccd1ed92ace1d41f0d8de8957"
string(40) "4b007901b765489abead49d926f721d065a429c1"
string(50) "3d2eec4fe41c849b80c8d83662c0e44a8b291a964cf2f07038"
string(32) "56fa6aa75548099dcc37d7f03425e0c3"
