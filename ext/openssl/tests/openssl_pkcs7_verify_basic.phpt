--TEST--
openssl_pkcs7_verify() tests
--EXTENSIONS--
openssl
--FILE--
<?php
$outfile = tempnam(sys_get_temp_dir(), "ssl");
if ($outfile === false) {
    die("failed to get a temporary filename!");
}

$contentfile = tempnam(sys_get_temp_dir(), "ssl");
if ($contentfile === false) {
    die("failed to get a temporary filename!");
}

$pkcsfile = __DIR__ . "/openssl_pkcs7_verify__pkcsfile.tmp";
$eml = __DIR__ . "/signed.eml";
$wrong = "wrong";
$empty = "";
$cainfo = array();

var_dump(openssl_pkcs7_verify($wrong, 0));
var_dump(openssl_pkcs7_verify($empty, 0));
var_dump(openssl_pkcs7_verify($eml, 0));
var_dump(openssl_pkcs7_verify($eml, 0, $empty));
var_dump(openssl_pkcs7_verify($eml, PKCS7_NOVERIFY, $outfile));
var_dump(openssl_pkcs7_verify($eml, PKCS7_NOVERIFY, $outfile, $cainfo, $outfile, $contentfile));
var_dump(openssl_pkcs7_verify($eml, PKCS7_NOVERIFY, $outfile, $cainfo, $outfile, $contentfile, $pkcsfile));
var_dump(file_get_contents($pkcsfile));

if (file_exists($outfile)) {
    echo "true\n";
    unlink($outfile);
}

if (file_exists($contentfile)) {
    echo "true\n";
    unlink($contentfile);
}
?>
--CLEAN--
<?php
unlink(__DIR__ . DIRECTORY_SEPARATOR . '/openssl_pkcs7_verify__pkcsfile.tmp');
?>
--EXPECTF--
int(-1)
int(-1)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
string(20%d) "-----BEGIN PKCS7-----
MIIFzQYJKoZIhvcNAQcCoIIFvjCCBboCAQExDzANBglghkgBZQMEAgEFADALBgkq
hkiG9w0BBwGgggNwMIIDbDCCAtWgAwIBAgIJAK7FVsxyN1CiMA0GCSqGSIb3DQEB
BQUAMIGBMQswCQYDVQQGEwJCUjEaMBgGA1UECBMRUmlvIEdyYW5kZSBkbyBTdWwx
FTATBgNVBAcTDFBvcnRvIEFsZWdyZTEeMBwGA1UEAxMVSGVucmlxdWUgZG8gTi4g
QW5nZWxvMR8wHQYJKoZIhvcNAQkBFhBobmFuZ2Vsb0BwaHAubmV0MB4XDTA4MDYz
MDEwMjg0M1oXDTA4MDczMDEwMjg0M1owgYExCzAJBgNVBAYTAkJSMRowGAYDVQQI
ExFSaW8gR3JhbmRlIGRvIFN1bDEVMBMGA1UEBxMMUG9ydG8gQWxlZ3JlMR4wHAYD
VQQDExVIZW5yaXF1ZSBkbyBOLiBBbmdlbG8xHzAdBgkqhkiG9w0BCQEWEGhuYW5n
ZWxvQHBocC5uZXQwgZ8wDQYJKoZIhvcNAQEBBQADgY0AMIGJAoGBAMteno+QK1ul
X4/WDAVBYfoTPRTze4SZLwgael4jwWTytj+8c5nNllrFELD6WjJzfjaoIMhCF4w4
I2bkWR6/PTqrvnv+iiiItHfKvJgYqIobUhkiKmWa2wL3mgqvNRIqTrTC4jWZuCkx
Q/ksqL9O/F6zk+aRS1d+KbPaqCR5Rw+lAgMBAAGjgekwgeYwHQYDVR0OBBYEFNt+
QHK9XDWF7CkpgRLoYmhqtz99MIG2BgNVHSMEga4wgauAFNt+QHK9XDWF7CkpgRLo
Ymhqtz99oYGHpIGEMIGBMQswCQYDVQQGEwJCUjEaMBgGA1UECBMRUmlvIEdyYW5k
ZSBkbyBTdWwxFTATBgNVBAcTDFBvcnRvIEFsZWdyZTEeMBwGA1UEAxMVSGVucmlx
dWUgZG8gTi4gQW5nZWxvMR8wHQYJKoZIhvcNAQkBFhBobmFuZ2Vsb0BwaHAubmV0
ggkArsVWzHI3UKIwDAYDVR0TBAUwAwEB/zANBgkqhkiG9w0BAQUFAAOBgQCP1GUn
StC0TBqngr3Kx+zSUW8KutKO0ORc5R8aV/x9LlaJrzPyQJgiPpu5hXogLSKRIHxQ
S3X2+Y0VvIpW72LWPVKPhYlNtO3oKnfoJGKin0eEhXRZMjfEW/kznY+ZZmNifV2r
8s+KhNAqI4PbClvn4vh8xF/9+eVEj+hM+0OflDGCAiEwggIdAgEBMIGPMIGBMQsw
CQYDVQQGEwJCUjEaMBgGA1UECBMRUmlvIEdyYW5kZSBkbyBTdWwxFTATBgNVBAcT
DFBvcnRvIEFsZWdyZTEeMBwGA1UEAxMVSGVucmlxdWUgZG8gTi4gQW5nZWxvMR8w
HQYJKoZIhvcNAQkBFhBobmFuZ2Vsb0BwaHAubmV0AgkArsVWzHI3UKIwDQYJYIZI
AWUDBAIBBQCggeQwGAYJKoZIhvcNAQkDMQsGCSqGSIb3DQEHATAcBgkqhkiG9w0B
CQUxDxcNMTcwNTIyMTAxMDU1WjAvBgkqhkiG9w0BCQQxIgQg37MSoxw91phVhwUO
MeurwtXAXK1ADEeYYl/7Bfmz4CsweQYJKoZIhvcNAQkPMWwwajALBglghkgBZQME
ASowCwYJYIZIAWUDBAEWMAsGCWCGSAFlAwQBAjAKBggqhkiG9w0DBzAOBggqhkiG
9w0DAgICAIAwDQYIKoZIhvcNAwICAUAwBwYFKw4DAgcwDQYIKoZIhvcNAwICASgw
DQYJKoZIhvcNAQEBBQAEgYAw4XcsQ4BIhEuRNspG8RqPE9ODCrTWwXPSQ4B9fzks
KUAsqcefO8AfifY+uuq3/k6Prhl23U5ILth/0fUAIGFLTcIZziaGTwbpgcmRSmNi
jBxatHyKVaGJNGqij5KRk8vhEpy5mwOzmkUzYa0r4teXjyfnKhI/h1vUrO3kKybC
5Q==
-----END PKCS7-----
"
true
true
