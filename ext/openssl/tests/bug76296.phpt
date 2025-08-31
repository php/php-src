--TEST--
Bug #76296 openssl_pkey_get_public does not respect open_basedir
--EXTENSIONS--
openssl
--FILE--
<?php
$dir = __DIR__ . '/bug76296_openbasedir';
$pem = 'file://' . __DIR__ . '/public.key';
if (!is_dir($dir)) {
    mkdir($dir);
}

ini_set('open_basedir', $dir);

var_dump(openssl_pkey_get_public($pem));
?>
--CLEAN--
<?php
@rmdir(__DIR__ . '/bug76296_openbasedir');
?>
--EXPECTF--
Warning: openssl_pkey_get_public(): open_basedir restriction in effect. File(%s) is not within the allowed path(s): (%s) in %s on line %d
bool(false)
