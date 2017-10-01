--TEST--
Bug #73792 (invalid foreach loop hangs script)
--FILE--
<?php
$a = 'aaa';

foreach ($a['bbb'] as &$value) {
	echo 'loop';
}

unset($value);
echo 'done';
?>
--EXPECTF--
Warning: Illegal string offset 'bbb' in %sbug73792.php on line 4

Fatal error: Uncaught Error: Cannot iterate on string offsets by reference in %sbug73792.php:4
Stack trace:
#0 {main}
  thrown in %sbug73792.php on line 4
