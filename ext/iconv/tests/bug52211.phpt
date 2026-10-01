--TEST--
Bug #52211 (iconv() returns part of string on error)
--EXTENSIONS--
iconv
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Solaris') {
    die("skip Solaris iconv behaves differently");
}
?>
--FILE--
<?php

// According to POSIX 2024, the to/from charset names are
// implementation-defined. To keep this test true to its original
// purpose, we retain the charsets used in bug 52211, but only when
// the implementation is known to support them. Otherwise we default
// both to ASCII, which should be supported everywhere (in particular
// on musl) yet still considers the input invalid.
$from_charset = "ASCII";
$to_charset = "ASCII";

if (ICONV_IMPL == "libiconv" || ICONV_IMPL == "glibc") {
    $from_charset = "CP850";
    $to_charset = "ISO-8859-1";
}

$str = "PATHOLOGIES MÉDICO-CHIRUR. ADUL. PL";
$str_iconv = iconv($from_charset, $to_charset, $str );
var_dump($str_iconv);

?>
--EXPECTF--
Notice: iconv(): Detected an illegal character in input string in %s on line %d
bool(false)
