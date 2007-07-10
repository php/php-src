--TEST--
declare script encoding (SJIS)
--SKIPIF--
<?php if (!unicode_semantics()) die('skip unicode.semantics=off'); ?>
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
