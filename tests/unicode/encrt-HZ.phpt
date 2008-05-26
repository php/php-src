--TEST--
declare script encoding (HZ)
--INI--
unicode.output_encoding=CP866
--FILE--
<?php
declare(encoding="HZ");

function ~{'d'V'c'd~}() {
  echo "~{'d'V'c'd~} - ok\n";
}

~{'d'V'c'd~}();
?>
--EXPECT--
тест - ok
