--TEST--
Request #77761 (openssl_x509_parse does not create entries for public key type and size)
--EXTENSIONS--
openssl
--FILE--
<?php

$certificate = <<<CERT
-----BEGIN CERTIFICATE-----
MIIB3zCCAYWgAwIBAgIUWoRHmceFhGk6IaNQUFDPYTnQ4I4wCgYIKoZIzj0EAwIw
RTELMAkGA1UEBhMCQVUxEzARBgNVBAgMClNvbWUtU3RhdGUxITAfBgNVBAoMGElu
dGVybmV0IFdpZGdpdHMgUHR5IEx0ZDAeFw0yNDExMDMxNjQzNDVaFw0yNTExMDMx
NjQzNDVaMEUxCzAJBgNVBAYTAkFVMRMwEQYDVQQIDApTb21lLVN0YXRlMSEwHwYD
VQQKDBhJbnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQwWTATBgcqhkjOPQIBBggqhkjO
PQMBBwNCAASGaV0Ce416vnEYfiC/nvq7FH8NhsrxCxIS2LVoCfpvSgmzH/klEfkT
omzMzA6cLbSFKGhfDWUwPlKF/XsxZ+oTo1MwUTAdBgNVHQ4EFgQUC3+muHeWsFHk
DtVB32pu2X/WIgAwHwYDVR0jBBgwFoAUC3+muHeWsFHkDtVB32pu2X/WIgAwDwYD
VR0TAQH/BAUwAwEB/zAKBggqhkjOPQQDAgNIADBFAiBo3HaOgyzimwwJApw5ijqI
9b8qdPx7kMsCFElxjYXn7QIhAMTWsw/AWP/Acq8YSQJMyUJ9H/ZmppHrZWLnQkc8
W+mj
-----END CERTIFICATE-----
CERT;

$publicKey = openssl_x509_parse($certificate)["publicKey"];
var_dump($publicKey["bits"]);
var_dump($publicKey["type"] === "EC" || $publicKey["type"] === "id-ecPublicKey");

if (OPENSSL_VERSION_NUMBER >= 0x30000000) {
    var_dump($publicKey["groupName"] === "prime256v1");
} else {
    var_dump(true);
}

?>
--EXPECT--
int(256)
bool(true)
bool(true)
