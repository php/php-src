--TEST--
Bug #52211 (iconv() returns part of string on error)
--SKIPIF--
<?php
// The two GNU iconvs (glibc and libiconv) fail if the input sequence
// cannot be converted to the target charset, but most others
// (including musl, FreeBSD, NetBSD, and Solaris) do not. POSIX says
// that "iconv() shall perform an implementation-defined conversion on
// the character," so this is likely to remain a GNUism.
if (ICONV_IMPL != "libiconv" && ICONV_IMPL != "glibc") {
    die("skip iconv impl may not fail on unconvertable sequences");
}
?>
--EXTENSIONS--
iconv
--FILE--
<?php

$str = "PATHOLOGIES MÉDICO-CHIRUR. ADUL. PL";
$str_iconv = iconv('CP850', 'ISO-8859-1', $str );
var_dump($str_iconv);

?>
--EXPECTF--
Notice: iconv(): Detected an illegal character in input string in %s on line %d
bool(false)
