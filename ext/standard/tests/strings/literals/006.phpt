--TEST--
Literal Variables in Constant Arrays
--FILE--
<?php
$array = ['aa' => 'aa', 'bb' => 'bb', 'cc' => 'cc'];

foreach ($array as $k => $v)
    var_dump(is_literal($k), is_literal($v));

echo PHP_EOL;

$array = ['aa' => 1, 'bb' => 2, 'cc' => 3];

foreach ($array as $k => $v)
    var_dump(is_literal($k), is_literal($v));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
