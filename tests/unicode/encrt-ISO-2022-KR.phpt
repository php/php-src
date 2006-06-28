--TEST--
declare script encoding (ISO-2022-KR)
--INI--
unicode.semantics=on
unicode.output_encoding=CP866
--FILE--
<?php
declare(encoding="ISO-2022-KR");

function ,d,V,c,d() {
  echo ",d,V,c,d - okÇn";
}

,d,V,c,d();
?>
--EXPECT--
‚•·‚ - ok
