--TEST--
registerPHPFunctions() with callables - error cases
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument();
$doc->loadHTML('<a href="https://php.net">hello</a>');

$xpath = new DOMXPath($doc);
try {
    $xpath->registerPhpFunctions("nonexistent");
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $xpath->registerPhpFunctions(function () {});
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $xpath->registerPhpFunctions([function () {}]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $xpath->registerPhpFunctions([var_dump(...)]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $xpath->registerPhpFunctions(["nonexistent"]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $xpath->registerPhpFunctions(["" => var_dump(...)]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $xpath->registerPhpFunctions(["\0" => var_dump(...)]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $xpath->registerPhpFunctions("");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$x = new class {
    public static function dump() {}
};

$classes = get_declared_classes();

try {
    $str = str_repeat($classes[count($classes) - 1] . '::dump', random_int(1, 1));
    $xpath->registerPhpFunctions([$str]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: DOMXPath::registerPhpFunctions(): Argument #1 ($restrict) must be a callable, function "nonexistent" not found or invalid function name
TypeError: DOMXPath::registerPhpFunctions(): Argument #1 ($restrict) must be of type array|string|null, Closure given
Error: Object of class Closure could not be converted to string
Error: Object of class Closure could not be converted to string
TypeError: DOMXPath::registerPhpFunctions(): Argument #1 ($restrict) must be an array with valid callbacks as values, function "nonexistent" not found or invalid function name
ValueError: DOMXPath::registerPhpFunctions(): Argument #1 ($restrict) must be an array containing valid callback names
ValueError: DOMXPath::registerPhpFunctions(): Argument #1 ($restrict) must be an array containing valid callback names
ValueError: DOMXPath::registerPhpFunctions(): Argument #1 ($restrict) must be a valid callback name
ValueError: DOMXPath::registerPhpFunctions(): Argument #1 ($restrict) must be an array containing valid callback names
