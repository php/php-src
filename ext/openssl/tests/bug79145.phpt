--TEST--
Bug #79145 (openssl memory leak)
--EXTENSIONS--
openssl
--FILE--
<?php
$b = '-----BEGIN PUBLIC KEY-----
MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDaFhc31WeskqxwI+Si5R/fZrLK
pJOlABiI3RZfKCHJVrXl3IvcHDFM/BHKUJoSi/ee8GS9iw0G4Z1eCzJdthXxHARh
j85Q5OliVxOdB1LoTOsOmfFf/fdvpU3DsOWsDKlVrL41MHxXorwrwOiys/r/gv2d
C9C4JmhTOjBVAK8SewIDAQAC
-----END PUBLIC KEY-----';

$a = openssl_get_publickey($b);
@openssl_free_key($a);

$start = memory_get_usage(true);
$a = openssl_get_publickey($b);
@openssl_free_key($a);
$end = memory_get_usage(true);
var_dump($end == $start);
?>
--EXPECT--
bool(true)
