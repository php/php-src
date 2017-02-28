--TEST--
Use of non-literals in declare ticks values crashes compiler
--FILE--
<?php

declare(ticks = UNKNOWN_CONST) {
  echo 'Done';
}
--EXPECTF--
Parse error: syntax error, unexpected 'UNKNOWN_CONST' (T_STRING), expecting integer number (T_LNUMBER) or floating-point number (T_DNUMBER) or quoted-string (T_CONSTANT_ENCAPSED_STRING) in %s/declare_006.php on line 3
