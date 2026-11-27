--TEST--
ctype and PCRE follow setlocale() between single byte and UTF-8 locales
--EXTENSIONS--
ctype
--SKIPIF--
<?php
if (setlocale(LC_CTYPE, 'invalid') === 'invalid') die('skip setlocale() accepts any locale (musl)');
if (!setlocale(LC_CTYPE, 'de_DE.ISO8859-1', 'de_DE.ISO-8859-1', 'de_DE.iso88591')) die('skip no ISO-8859-1 locale');
if (!setlocale(LC_CTYPE, 'de_DE.UTF-8', 'en_US.UTF-8', 'C.UTF-8')) die('skip no UTF-8 locale');
if (PHP_OS_FAMILY === 'Darwin') die('skip isalpha() of macOS knows the bytes from 128 on in UTF-8 locales');
?>
--FILE--
<?php

// 0xE9 is a letter in ISO-8859-1 and no character in UTF-8
$locales = [
    'C' => ['C'],
    'ISO-8859-1' => ['de_DE.ISO8859-1', 'de_DE.ISO-8859-1', 'de_DE.iso88591'],
    'UTF-8' => ['de_DE.UTF-8', 'en_US.UTF-8', 'C.UTF-8'],
];

foreach (['C', 'ISO-8859-1', 'UTF-8', 'ISO-8859-1', 'C', 'UTF-8'] as $charset) {
    setlocale(LC_CTYPE, $locales[$charset]);
    echo $charset, ': ';
    var_dump(ctype_alpha("\xE9"), preg_match('/^\w$/', "\xE9"));
}

?>
--EXPECT--
C: bool(false)
int(0)
ISO-8859-1: bool(true)
int(1)
UTF-8: bool(false)
int(0)
ISO-8859-1: bool(true)
int(1)
C: bool(false)
int(0)
UTF-8: bool(false)
int(0)
