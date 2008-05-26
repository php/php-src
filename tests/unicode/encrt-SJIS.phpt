--TEST--
declare script encoding (SJIS)
--INI--
unicode.output_encoding=CP866
--FILE--
<?php
declare(encoding="SJIS");

function ÑÑÑuÑÉÑÑ() {
  echo "ÑÑÑuÑÉÑÑ - ok\n";
}

ÑÑÑuÑÉÑÑ();
?>
--EXPECT--
‚•·‚ - ok
