--TEST--
Bug #79821 (array grow during var_dump)
--FILE--
<?php

$foo = $bar = [];
for ($i = 0; $i < 3; $i++) {
    $foo = [$foo, [&$bar]];
}
ob_start(function (string $buffer) use (&$bar) {
    $bar[][] = null;
    return '';
}, 1);
var_dump($foo[0]);
ob_end_clean();

echo "OK\n";

?>
--EXPECT--
OK
