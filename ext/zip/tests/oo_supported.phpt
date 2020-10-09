--TEST--
ziparchive::properties isset()/empty() checks
--SKIPIF--
<?php
/* $Id$ */
if(!extension_loaded('zip')) die('skip');
if (!method_exists('ZipArchive', 'isCompressionMethodSupported')) die('skip needs libzip >= 1.7');
?>
--FILE--
<?php
$methods = [
    ZipArchive::CM_STORE      => "STORE",
    ZipArchive::CM_DEFLATE    => "DEFLATE",
    ZipArchive::CM_BZIP2      => "BZIP2",
    ZipArchive::CM_XZ         => "XZ",
];
foreach($methods as $method => $name) {
    echo "Compression $name\n";
    var_dump(ZipArchive::isCompressionMethodSupported($method));
    var_dump(ZipArchive::isCompressionMethodSupported($method, false));
}

$methods = [
    ZipArchive::EM_NONE        => "NONE",
    ZipArchive::EM_TRAD_PKWARE => "TRAD_PKWARE",
    ZipArchive::EM_AES_128     => "AES-128",
    ZipArchive::EM_AES_192     => "AES-192",
    ZipArchive::EM_AES_256     => "AES-256",
];
foreach($methods as $method => $name) {
    echo "Encryption $name\n";
    var_dump(ZipArchive::isEncryptionMethodSupported($method));
    var_dump(ZipArchive::isEncryptionMethodSupported($method, false));
}
?>
Done
--EXPECTF--
Compression STORE
bool(true)
bool(true)
Compression DEFLATE
bool(true)
bool(true)
Compression BZIP2
bool(%s)
bool(%s)
Compression XZ
bool(%s)
bool(%s)
Encryption NONE
bool(true)
bool(true)
Encryption TRAD_PKWARE
bool(true)
bool(true)
Encryption AES-128
bool(%s)
bool(%s)
Encryption AES-192
bool(%s)
bool(%s)
Encryption AES-256
bool(%s)
bool(%s)
Done
