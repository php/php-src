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
Fatal error: Uncaught Error: Cannot create references to/from string offsets in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
