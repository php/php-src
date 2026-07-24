--TEST--
GH-22878 (Use-after-free of callable via autoloader)
--FILE--
<?php
spl_autoload_register(function (string $class): void {
    $GLOBALS['cb'] = null;
    eval("class $class { public static function __callStatic(\$name, \$args) { echo strlen(\$name), \"\\n\"; } }");
});

$method = str_repeat('m', 256);
$cb = ['GH22878ArrayCuf', $method];
unset($method);
call_user_func($cb);

$method = str_repeat('m', 256);
$cb = ['GH22878ArrayCufa', $method];
unset($method);
call_user_func_array($cb, []);

$method = str_repeat('m', 256);
$cb = ['GH22878ArrayDynamic', $method];
unset($method);
$cb();

$method = str_repeat('m', 256);
$cb = 'GH22878Str::' . $method;
unset($method);
call_user_func($cb);

$cb = 'GH22878Lit::literalMethod';
call_user_func($cb);

echo "done\n";
?>
--EXPECT--
256
256
256
256
13
done
