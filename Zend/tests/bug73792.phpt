--TEST--
Bug #73792 (invalid foreach loop hangs script)
--FILE--
<?php
$a = 'aaa';

foreach ($a['2bbb'] as &$value) {
    echo 'loop';
}

unset($value);
echo 'done';
?>
--EXPECTF--
Warning: Illegal string offset "2bbb" in %s on line %d

Fatal error: Uncaught Error: Cannot create references to/from string offsets in %s:%d
Stack trace:
#0 {main}
  thrown in %sbug73792.php on line 4
