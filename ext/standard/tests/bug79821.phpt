--TEST--
Bug #79821 (array grow during var_dump)
--FILE--
<?php

foreach (['var_dump', 'debug_zval_dump', 'var_export'] as $output) {
    $foo = $bar = [];
    for ($i = 0; $i < 3; $i++) {
        $foo = [$foo, [&$bar]];
    }
    ob_start(function (string $buffer) use (&$bar) {
        $bar[][] = null;
        return '';
    }, 64);
    $output($foo[0]);
    ob_end_clean();
}

echo "OK\n";

?>
--EXPECT--
OK
