--TEST--
declare script encoding (ISO-2022-KR)
--SKIPIF--
<?php if (!unicode_semantics()) die('skip unicode.semantics=off'); ?>
--INI--
unicode.output_encoding=CP866
--FILE--
<?php
declare(encoding="ISO-2022-KR");

function ,d,V,c,d() {
  echo ",d,V,c,d - ok‚n";
}

,d,V,c,d();
?>
--EXPECT--
β¥αβ - ok
