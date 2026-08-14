--TEST--
Constant argument optimization - strict_types
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.file_update_protection=0
--FILE--
<?php
declare(strict_types=1);

function f(int $a, $b) { return $a; }

// Check that optimizations are enabled
$f = f(3, ?);
$usedVars = new ReflectionFunction($f)->getClosureUsedVariables();
if ($usedVars !== []) {
    echo "const arg optimization was not applied\n";
    var_dump($usedVars);
    exit;
}

var_dump($f(0));

// Actual test. Not catching this as it disables optimizations
f("3", ?)(0);

?>
--EXPECTF--
int(3)

Fatal error: Uncaught TypeError: f(): Argument #1 ($a) must be of type int, string given in %a
