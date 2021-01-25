--TEST--
Test class: serialize in supported RNG classes in 64bit range.
--SKIPIF--
<?php
if (PHP_INT_SIZE < 8) {
    echo 'skip this test only available on 64bit enviroment.';
}
?>
--FILE--
<?php
foreach (include(__DIR__ . DIRECTORY_SEPARATOR . '_serializable_rng_classes.inc') as $class => $is_seed) {
    $e = \explode('\\', $class);
    $define_class = 'X' . \end($e);
    eval("class ${define_class} extends ${class} { public \$foo; public \$bar; public \$baz; }");
    $t = $is_seed ? new $class(\random_int(PHP_INT_MIN, PHP_INT_MAX)) : new $class();
    $t->foo = 'bar';
    $t->bar = 1234;
    $t->baz = [1, 2, 3, 4];
    \rng_next64($t);

    $s = \serialize($t);
    $t_next64 = \rng_next64($t);
    $ut = \unserialize($s);
    $ut_next64 = \rng_next64($ut);
    if ($ut_next64 !== $t_next64) {
        die("NG, broken detected. class: ${class} method: next64() correct: ${t_next64} result: ${ut_next64}");
    }
    if ($ut->foo !== 'bar' ||
        $ut->bar !== 1234 ||
        $ut->baz[0] !== 1 ||
        $ut->baz[1] !== 2 ||
        $ut->baz[2] !== 3 ||
        $ut->baz[3] !== 4
    ) {
        die('NG, broken detected in properties: ' . \print_r($ut, true));
    }
}
die('OK, serialize / unserialize works correctly.');
?>
--EXPECT--
OK, serialize / unserialize works correctly.
