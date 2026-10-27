--TEST--
GH-24784 (openssl_pkcs7_read()/openssl_cms_read() CRLs overwrite certificates)
--EXTENSIONS--
openssl
--FILE--
<?php
$pkcs7 = <<<'PEM'
-----BEGIN PKCS7-----
MIIEmAYJKoZIhvcNAQcCoIIEiTCCBIUCAQExADALBgkqhkiG9w0BBwGgggL/MIIC
+zCCAeOgAwIBAgIUUEaeiRSbQNAJSjCJETx/k53qiDIwDQYJKoZIhvcNAQELBQAw
DDEKMAgGA1UEAwwBdDAgFw0yNjEwMjcxMjQ1MTRaGA8yMTI2MTAwMzEyNDUxNFow
DDEKMAgGA1UEAwwBdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAIh+
9ye+esW5wXVu7ERwOEJMci0+Ivci+2uA6nxmDmJARJSGn4sqB6+g1MLMzfxcMECi
OKowxoWfmWvlluPCMrRI732tmAcXt+x1gdZlUrzvi/ul3SPxgIv/pqNZ2DxTlgpb
4cBooSFFKLUcHO5MrqXiDkL5MFLG6165N/OKBtI3ylu9pwgKOKGm0vIwanStMcpP
nBjpjJWLJpzQdUjbC7s9sV3iODM8qHC5izl5ueDY2d4NPutHJxE3olF5fad/IX4N
pbOVzVK2aIMVN9Q+nCoZlZtkg2B7RCL6WzVfxtmVout8ELAaHKKor5SZYuhOe7Rb
O5cebVYYkIZVXe8CPB0CAwEAAaNTMFEwHQYDVR0OBBYEFNGPW5eEb7Kz8RGrRP0d
1ABhu9xeMB8GA1UdIwQYMBaAFNGPW5eEb7Kz8RGrRP0d1ABhu9xeMA8GA1UdEwEB
/wQFMAMBAf8wDQYJKoZIhvcNAQELBQADggEBAFuyJeajbHMjr4KWDuAqAjS6y14p
IVi931dudVpqi5c8OOgOkX4eUF3Z5y9WkPozuYsx+u+pPs1iFVvzG13SjRqZu7tx
fyqIbZIy4VbWG2ZUTvVW34D5Y7Yj5FM9IeRyw2vuwL6NKw/6ENVIHJZRokHQ7zCQ
A9uBPt5rultYIsiRukaRgfu8P56wBXgAryN6bfWgNvFNwLwh9j8a/6tAsnoC0npS
MYiCowEXuROYmGvUPtZsv7Y8gspsbKgVBrOs8gqnCer4LXNVskQixfGQ8AyZNSSl
GwpkD7DIXaHg4V7upfnXcyY6HJgyG0i2oMCzlCpTRcf+OAK4KnRlowfCxoChggFq
MIIBZjBQAgEBMA0GCSqGSIb3DQEBCwUAMAwxCjAIBgNVBAMMAXQXDTI2MTAyNzEy
NDUxNFoYDzIxMjYxMDAzMTI0NTE0WqAOMAwwCgYDVR0UBAMCAQEwDQYJKoZIhvcN
AQELBQADggEBAHCQBFS6x6N9v5LA/zWtHY/ZRt3ZNWw9kh4QrjKZA+c2WN36WrIz
l65ApT2OOlF7PxHrLZXxDnBe/IxlotAOyChWemRBj0Tb6Rjl5ktKNnXUrmgpH4uB
m9dPNs/b2tlIoYECbwHKQcYRaGJE0/BxDRXkAq6fUoHJVH1zXLZOc+8IHeDnqThe
IxKcfS1JwUQ4Fjjd8Po1EA9puFqhWE2oxD1E4C4gaRul1gQXDcLFlqMM8zISRHEl
1IR6DCP25u9vzN2mxSGaeY1awDUeWopkz1d3PZgZRxqSh5ojqvYRC7oxHyhWqiut
+RY+Dyn3ETAoatWiH2GOEnoLNZtVEK3C3UYxAA==
-----END PKCS7-----
PEM;
$cms = str_replace('PKCS7', 'CMS', $pkcs7);

foreach (['openssl_pkcs7_read' => $pkcs7, 'openssl_cms_read' => $cms] as $func => $data) {
    echo $func, "\n";
    var_dump($func($data, $result));
    var_dump(count($result));
    foreach ($result as $i => $pem) {
        echo $i, ': ', strtok($pem, "\n"), "\n";
    }
}
?>
--EXPECT--
openssl_pkcs7_read
bool(true)
int(2)
0: -----BEGIN CERTIFICATE-----
1: -----BEGIN X509 CRL-----
openssl_cms_read
bool(true)
int(2)
0: -----BEGIN CERTIFICATE-----
1: -----BEGIN X509 CRL-----
