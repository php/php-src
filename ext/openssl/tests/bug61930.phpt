--TEST--
Bug #61930: openssl corrupts ssl key resource when using openssl_get_publickey()
--EXTENSIONS--
openssl
--FILE--
<?php
$cert = file_get_contents(__DIR__.'/public.crt');

$data = <<<DATA
Please verify me
DATA;

$sig = 'w45LtLoRY/WPk/kcmP6CwGysOMuxuLbD35xMB/iAe5IMiBJjz2D1WGEY7Vz+rLZmYqOo58qNC3VtTg6ge9+UhfQHplvs6cXGKm' .
       'SkQlYv4EeFoByqYfPU9k2dE/WEItOJUUyqu9pHaCmRtLpxoLnJcdQVdiXfT0t8KwxUzZYDjrSfhp7rbKhhCc4jZMwo9PvBuPAT' .
       'MEfZbRNaVpwCFpjmmJczZCHFZFm7JYzR2jU0sjJMGALXidNBs9p0Fi1TGz3pZkxnQ5lwI5DX5ZSY0jiOcoVFt7k29GVFd0DPjm' .
       '1NyieYU6tpnanG+ZqHIT8Um3FajYh0x1iMMe2lLETjklqYiw==';

$key = openssl_get_publickey($cert);
var_dump(openssl_get_publickey($key));
var_dump(openssl_verify($data, base64_decode($sig), $key, OPENSSL_ALGO_SHA256));
?>
--EXPECTF--
object(OpenSSLAsymmetricKey)#%d (0) {
}
int(1)
