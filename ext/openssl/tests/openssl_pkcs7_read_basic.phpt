--TEST--
openssl_pkcs7_read() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$infile = file_get_contents(__DIR__ . "/cert.p7b");
$certfile = file_get_contents(__DIR__ . "/cert.crt");
$result = [];

var_dump(openssl_pkcs7_read("", $result));
var_dump(openssl_pkcs7_read($certfile, $result));
var_dump(openssl_pkcs7_read($infile, $result));
var_dump($result);
?>
--EXPECT--
bool(false)
bool(false)
bool(true)
array(1) {
  [0]=>
  string(1249) "-----BEGIN CERTIFICATE-----
MIIDbDCCAtWgAwIBAgIJAK7FVsxyN1CiMA0GCSqGSIb3DQEBBQUAMIGBMQswCQYD
VQQGEwJCUjEaMBgGA1UECBMRUmlvIEdyYW5kZSBkbyBTdWwxFTATBgNVBAcTDFBv
cnRvIEFsZWdyZTEeMBwGA1UEAxMVSGVucmlxdWUgZG8gTi4gQW5nZWxvMR8wHQYJ
KoZIhvcNAQkBFhBobmFuZ2Vsb0BwaHAubmV0MB4XDTA4MDYzMDEwMjg0M1oXDTA4
MDczMDEwMjg0M1owgYExCzAJBgNVBAYTAkJSMRowGAYDVQQIExFSaW8gR3JhbmRl
IGRvIFN1bDEVMBMGA1UEBxMMUG9ydG8gQWxlZ3JlMR4wHAYDVQQDExVIZW5yaXF1
ZSBkbyBOLiBBbmdlbG8xHzAdBgkqhkiG9w0BCQEWEGhuYW5nZWxvQHBocC5uZXQw
gZ8wDQYJKoZIhvcNAQEBBQADgY0AMIGJAoGBAMteno+QK1ulX4/WDAVBYfoTPRTz
e4SZLwgael4jwWTytj+8c5nNllrFELD6WjJzfjaoIMhCF4w4I2bkWR6/PTqrvnv+
iiiItHfKvJgYqIobUhkiKmWa2wL3mgqvNRIqTrTC4jWZuCkxQ/ksqL9O/F6zk+aR
S1d+KbPaqCR5Rw+lAgMBAAGjgekwgeYwHQYDVR0OBBYEFNt+QHK9XDWF7CkpgRLo
Ymhqtz99MIG2BgNVHSMEga4wgauAFNt+QHK9XDWF7CkpgRLoYmhqtz99oYGHpIGE
MIGBMQswCQYDVQQGEwJCUjEaMBgGA1UECBMRUmlvIEdyYW5kZSBkbyBTdWwxFTAT
BgNVBAcTDFBvcnRvIEFsZWdyZTEeMBwGA1UEAxMVSGVucmlxdWUgZG8gTi4gQW5n
ZWxvMR8wHQYJKoZIhvcNAQkBFhBobmFuZ2Vsb0BwaHAubmV0ggkArsVWzHI3UKIw
DAYDVR0TBAUwAwEB/zANBgkqhkiG9w0BAQUFAAOBgQCP1GUnStC0TBqngr3Kx+zS
UW8KutKO0ORc5R8aV/x9LlaJrzPyQJgiPpu5hXogLSKRIHxQS3X2+Y0VvIpW72LW
PVKPhYlNtO3oKnfoJGKin0eEhXRZMjfEW/kznY+ZZmNifV2r8s+KhNAqI4PbClvn
4vh8xF/9+eVEj+hM+0OflA==
-----END CERTIFICATE-----
"
}
