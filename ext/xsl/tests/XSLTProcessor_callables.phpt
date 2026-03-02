--TEST--
registerPhpFunctions() with callables - legit cases
--EXTENSIONS--
xsl
--FILE--
<?php

require __DIR__ . '/xpath_callables.inc';

$inputdom = new DomDocument();
$inputdom->loadXML('<?xml version="1.0" encoding="iso-8859-1"?><a href="https://php.net">hello</a>');

echo "--- Legit cases: none ---\n";

$proc = createProcessor(["'var_dump', string(@href)"]);
try {
    $proc->transformToXml($inputdom);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

echo "--- Legit cases: all ---\n";

$proc = createProcessor(["'var_dump', string(@href)", "'MyClass::dump', string(@href)"]);
$proc->registerPHPFunctions();
var_dump($proc->transformToXml($inputdom));

echo "--- Legit cases: set ---\n";

$proc = createProcessor(["'mydump', string(@href)", "'xyz', string(@href)", "'var_dump', string(@href)"]);
$proc->registerPhpFunctions([]);
$proc->registerPHPFunctions(["xyz" => MyClass::dump(...), "mydump" => function (string $x) {
    var_dump($x);
}]);
$proc->registerPhpFunctions(str_repeat("var_dump", mt_rand(1, 1) /* defeat SCCP */));
var_dump($proc->transformToXml($inputdom));

$proc = createProcessor(["'notinset', string(@href)"]);
$proc->registerPhpFunctions([]);
try {
    var_dump($proc->transformToXml($inputdom));
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

echo "--- Legit cases: set with cycle ---\n";

$proc = createProcessor(["'cycle', string(@href)"], 'MyXSLTProcessor');
$proc->registerCycle();
var_dump($proc->transformToXml($inputdom));

?>
--EXPECT--
--- Legit cases: none ---
No callbacks were registered
--- Legit cases: all ---
string(15) "https://php.net"
string(15) "https://php.net"
string(44) "<?xml version="1.0"?>
dump: https://php.net
"
--- Legit cases: set ---
string(15) "https://php.net"
string(15) "https://php.net"
string(15) "https://php.net"
string(44) "<?xml version="1.0"?>
dump: https://php.net
"
No callback handler "notinset" registered
--- Legit cases: set with cycle ---
string(45) "<?xml version="1.0"?>
dummy: https://php.net
"
