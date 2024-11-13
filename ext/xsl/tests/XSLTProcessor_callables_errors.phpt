--TEST--
registerPhpFunctions() with callables - error cases
--EXTENSIONS--
xsl
--FILE--
<?php

require __DIR__ . '/xpath_callables.inc';

$proc = createProcessor([]);
try {
    $proc->registerPhpFunctions("nonexistent");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $proc->registerPhpFunctions(function () {});
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $proc->registerPhpFunctions([function () {}]);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

try {
    $proc->registerPhpFunctions([var_dump(...)]);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

try {
    $proc->registerPhpFunctions(["nonexistent"]);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

try {
    $proc->registerPhpFunctions(["" => var_dump(...)]);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

try {
    $proc->registerPhpFunctions(["\0" => var_dump(...)]);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

try {
    $proc->registerPhpFunctions("");
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
XSLTProcessor::registerPHPFunctions(): Argument #1 ($functions) must be a callable, function "nonexistent" not found or invalid function name
XSLTProcessor::registerPHPFunctions(): Argument #1 ($functions) must be of type array|string|null, Closure given
Object of class Closure could not be converted to string
Object of class Closure could not be converted to string
XSLTProcessor::registerPHPFunctions(): Argument #1 ($functions) must be an array with valid callbacks as values, function "nonexistent" not found or invalid function name
XSLTProcessor::registerPHPFunctions(): Argument #1 ($functions) must be an array containing valid callback names
XSLTProcessor::registerPHPFunctions(): Argument #1 ($functions) must be an array containing valid callback names
XSLTProcessor::registerPHPFunctions(): Argument #1 ($functions) must be a valid callback name
