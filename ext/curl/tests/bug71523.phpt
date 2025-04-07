--TEST--
Bug #71523 (Copied handle with new option CURLOPT_HTTPHEADER crashes while curl_multi_exec)
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] === 0x080a00) {
    // https://github.com/php/php-src/issues/15997
    die('xfail due to a libcurl bug');
}
?>
--FILE--
<?php

$base = curl_init('http://www.google.com/');
curl_setopt($base, CURLOPT_RETURNTRANSFER, true);
$mh = curl_multi_init();

for ($i = 0; $i < 2; ++$i) {
    $ch = curl_copy_handle($base);
    curl_setopt($ch, CURLOPT_HTTPHEADER, ['Foo: Bar']);
    curl_multi_add_handle($mh, $ch);
}

do {
    curl_multi_exec($mh, $active);
} while ($active);
?>
okey
--EXPECT--
okey
