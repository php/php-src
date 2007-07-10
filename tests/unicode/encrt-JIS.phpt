--TEST--
declare script encoding (JIS)
--SKIPIF--
<?php if (!unicode_semantics()) die('skip unicode.semantics=off'); ?>
--INI--
unicode.output_encoding=CP866
--FILE--
<?php
declare(encoding="JIS");

function $B'd'V'c'd(B() {
  echo "$B'd'V'c'd(B - ok\n";
}

$B'd'V'c'd(B();
?>
--EXPECT--
тест - ok
