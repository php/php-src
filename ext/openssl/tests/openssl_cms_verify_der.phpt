--TEST--
openssl_cms_verify() der tests
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

$pkcsfile = __DIR__ . "/openssl_cms_verify__pkcsfile.tmp";
$eml = __DIR__ . "/signed.eml";
$wrong = "wrong";
$empty = "";
$cainfo = array();
$plain = __DIR__ . "/plain.txt";
$p7file = __DIR__ . "/shakespeare.p7s";
$pemfile = __DIR__ . "/shakespeare.pem";
var_dump(openssl_cms_verify($wrong, 0));
var_dump(openssl_cms_verify($empty, 0));
var_dump(openssl_cms_verify($eml, 0));
var_dump(openssl_cms_verify($eml, 0, $empty));
var_dump(openssl_cms_verify($eml, OPENSSL_CMS_NOVERIFY, $outfile));
var_dump(openssl_cms_verify($eml, OPENSSL_CMS_NOVERIFY, $outfile, $cainfo, $outfile, $contentfile));
var_dump(openssl_cms_verify($eml, OPENSSL_CMS_NOVERIFY, $outfile, $cainfo, $outfile, $contentfile, $pkcsfile));
var_dump(openssl_cms_verify($plain, OPENSSL_CMS_NOVERIFY|OPENSSL_CMS_BINARY|OPENSSL_CMS_DETACHED,$outfile,$cainfo,NULL,$contentfile,$pkcsfile,$p7file,OPENSSL_ENCODING_DER));
var_dump(openssl_cms_verify($plain, OPENSSL_CMS_NOVERIFY|OPENSSL_CMS_BINARY|OPENSSL_CMS_DETACHED,$outfile,$cainfo,NULL,$contentfile,$pkcsfile,$pemfile,OPENSSL_ENCODING_PEM));
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
unlink(__DIR__ . DIRECTORY_SEPARATOR . '/openssl_cms_verify__pkcsfile.tmp');
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
string(2049) "-----BEGIN CMS-----
MIIFyQYJKoZIhvcNAQcCoIIFujCCBbYCAQExDTALBglghkgBZQMEAgEwCwYJKoZI
hvcNAQcBoIIDcDCCA2wwggLVoAMCAQICCQCuxVbMcjdQojANBgkqhkiG9w0BAQUF
ADCBgTELMAkGA1UEBhMCQlIxGjAYBgNVBAgTEVJpbyBHcmFuZGUgZG8gU3VsMRUw
EwYDVQQHEwxQb3J0byBBbGVncmUxHjAcBgNVBAMTFUhlbnJpcXVlIGRvIE4uIEFu
Z2VsbzEfMB0GCSqGSIb3DQEJARYQaG5hbmdlbG9AcGhwLm5ldDAeFw0wODA2MzAx
MDI4NDNaFw0wODA3MzAxMDI4NDNaMIGBMQswCQYDVQQGEwJCUjEaMBgGA1UECBMR
UmlvIEdyYW5kZSBkbyBTdWwxFTATBgNVBAcTDFBvcnRvIEFsZWdyZTEeMBwGA1UE
AxMVSGVucmlxdWUgZG8gTi4gQW5nZWxvMR8wHQYJKoZIhvcNAQkBFhBobmFuZ2Vs
b0BwaHAubmV0MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDLXp6PkCtbpV+P
1gwFQWH6Ez0U83uEmS8IGnpeI8Fk8rY/vHOZzZZaxRCw+loyc342qCDIQheMOCNm
5Fkevz06q757/oooiLR3yryYGKiKG1IZIiplmtsC95oKrzUSKk60wuI1mbgpMUP5
LKi/Tvxes5PmkUtXfimz2qgkeUcPpQIDAQABo4HpMIHmMB0GA1UdDgQWBBTbfkBy
vVw1hewpKYES6GJoarc/fTCBtgYDVR0jBIGuMIGrgBTbfkByvVw1hewpKYES6GJo
arc/faGBh6SBhDCBgTELMAkGA1UEBhMCQlIxGjAYBgNVBAgTEVJpbyBHcmFuZGUg
ZG8gU3VsMRUwEwYDVQQHEwxQb3J0byBBbGVncmUxHjAcBgNVBAMTFUhlbnJpcXVl
IGRvIE4uIEFuZ2VsbzEfMB0GCSqGSIb3DQEJARYQaG5hbmdlbG9AcGhwLm5ldIIJ
AK7FVsxyN1CiMAwGA1UdEwQFMAMBAf8wDQYJKoZIhvcNAQEFBQADgYEAj9RlJ0rQ
tEwap4K9ysfs0lFvCrrSjtDkXOUfGlf8fS5Wia8z8kCYIj6buYV6IC0ikSB8UEt1
9vmNFbyKVu9i1j1Sj4WJTbTt6Cp36CRiop9HhIV0WTI3xFv5M52PmWZjYn1dq/LP
ioTQKiOD2wpb5+L4fMRf/fnlRI/oTPtDn5QxggIfMIICGwIBATCBjzCBgTELMAkG
A1UEBhMCQlIxGjAYBgNVBAgTEVJpbyBHcmFuZGUgZG8gU3VsMRUwEwYDVQQHEwxQ
b3J0byBBbGVncmUxHjAcBgNVBAMTFUhlbnJpcXVlIGRvIE4uIEFuZ2VsbzEfMB0G
CSqGSIb3DQEJARYQaG5hbmdlbG9AcGhwLm5ldAIJAK7FVsxyN1CiMAsGCWCGSAFl
AwQCAaCB5DAYBgkqhkiG9w0BCQMxCwYJKoZIhvcNAQcBMBwGCSqGSIb3DQEJBTEP
Fw0yMDAzMTMxMDMxMThaMC8GCSqGSIb3DQEJBDEiBCD7i6er2GffWfJgrK5Yi2Kh
XfedL51/uszuPjxImv7g1jB5BgkqhkiG9w0BCQ8xbDBqMAsGCWCGSAFlAwQBKjAL
BglghkgBZQMEARYwCwYJYIZIAWUDBAECMAoGCCqGSIb3DQMHMA4GCCqGSIb3DQMC
AgIAgDANBggqhkiG9w0DAgIBQDAHBgUrDgMCBzANBggqhkiG9w0DAgIBKDANBgkq
hkiG9w0BAQEFAASBgBphoY0L/jnrzSIOpaS0b2Yzfw9CuuShcUA3oiib/tFn3mzV
gSfYaiIdL0d4FSMRiLFkkbBrxBKD+oMZIMI7ZurpQpqKq0OGPzORflh3sfFLqSI1
8Txoc8qJ5JKpip3W9IQwIwPpAU4HhBgWP2HPODY2/Q3bDtUQ3I9KrJIewr9Q
-----END CMS-----
"
true
true
