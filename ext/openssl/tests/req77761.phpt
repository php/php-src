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

print_r(openssl_x509_parse($certificate));

?>
--EXPECT--
Array
(
    [name] => /C=AU/ST=Some-State/O=Internet Widgits Pty Ltd
    [publicKey] => Array
        (
            [bits] => 256
            [type] => EC
            [groupName] => prime256v1
        )

    [subject] => Array
        (
            [C] => AU
            [ST] => Some-State
            [O] => Internet Widgits Pty Ltd
        )

    [hash] => 9da13359
    [issuer] => Array
        (
            [C] => AU
            [ST] => Some-State
            [O] => Internet Widgits Pty Ltd
        )

    [version] => 2
    [serialNumber] => 0x5A844799C78584693A21A3505050CF6139D0E08E
    [serialNumberHex] => 5A844799C78584693A21A3505050CF6139D0E08E
    [validFrom] => 241103164345Z
    [validTo] => 251103164345Z
    [validFrom_time_t] => 1730652225
    [validTo_time_t] => 1762188225
    [signatureTypeSN] => ecdsa-with-SHA256
    [signatureTypeLN] => ecdsa-with-SHA256
    [signatureTypeNID] => 794
    [purposes] => Array
        (
            [1] => Array
                (
                    [0] => 1
                    [1] => 1
                    [2] => sslclient
                )

            [2] => Array
                (
                    [0] => 1
                    [1] => 1
                    [2] => sslserver
                )

            [3] => Array
                (
                    [0] => 1
                    [1] => 1
                    [2] => nssslserver
                )

            [4] => Array
                (
                    [0] => 1
                    [1] => 1
                    [2] => smimesign
                )

            [5] => Array
                (
                    [0] => 1
                    [1] => 1
                    [2] => smimeencrypt
                )

            [6] => Array
                (
                    [0] => 1
                    [1] => 1
                    [2] => crlsign
                )

            [7] => Array
                (
                    [0] => 1
                    [1] => 1
                    [2] => any
                )

            [8] => Array
                (
                    [0] => 1
                    [1] => 1
                    [2] => ocsphelper
                )

            [9] => Array
                (
                    [0] => 
                    [1] => 1
                    [2] => timestampsign
                )

            [10] => Array
                (
                    [0] => 
                    [1] => 1
                    [2] => codesign
                )

        )

    [extensions] => Array
        (
            [subjectKeyIdentifier] => 0B:7F:A6:B8:77:96:B0:51:E4:0E:D5:41:DF:6A:6E:D9:7F:D6:22:00
            [authorityKeyIdentifier] => 0B:7F:A6:B8:77:96:B0:51:E4:0E:D5:41:DF:6A:6E:D9:7F:D6:22:00
            [basicConstraints] => CA:TRUE
        )

)
