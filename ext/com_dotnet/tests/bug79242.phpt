--TEST--
Bug #79242 (COM error constants don't match com_exception codes)
--EXTENSIONS--
com_dotnet
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platforms only");
?>
--FILE--
<?php
var_dump(
    DISP_E_DIVBYZERO,
    DISP_E_OVERFLOW,
    DISP_E_BADINDEX,
    MK_E_UNAVAILABLE
);
?>
--EXPECT--
int(-2147352558)
int(-2147352566)
int(-2147352565)
int(-2147221021)

