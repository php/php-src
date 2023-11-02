--TEST--
registerPhpFunctions() with callables
--EXTENSIONS--
xsl
--FILE--
<?php

class MyClass {
    public static function dump(string $var) {
        var_dump($var);
        return "dump: $var";
    }
}

class MyXSLTProcessor extends XSLTProcessor {
    public function registerCycle() {
        $this->registerPhpFunctions(["cycle" => array($this, "dummy")]);
    }

    public function dummy(string $var) {
        return "dummy: $var";
    }
}

function createProcessor($inputs, $class = "XSLTProcessor") {
    $xsl = new DomDocument();
    $xsl->loadXML('<?xml version="1.0" encoding="iso-8859-1" ?>
    <xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:php="http://php.net/xsl">
    <xsl:template match="//a">'
    . implode('', array_map(fn($input) => '<xsl:value-of select="php:function(' . $input . ')" />', $inputs)) .
    '</xsl:template>
    </xsl:stylesheet>');

    $proc = new $class();
    $proc->importStylesheet($xsl);
    return $proc;
}

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

echo "--- Error cases ---\n";

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
--- Error cases ---
XSLTProcessor::registerPHPFunctions(): Argument #1 ($functions) must be a callable, function "nonexistent" not found or invalid function name
XSLTProcessor::registerPHPFunctions(): Argument #1 ($functions) must be of type array|string|null, Closure given
Object of class Closure could not be converted to string
Object of class Closure could not be converted to string
XSLTProcessor::registerPHPFunctions(): Argument #1 ($functions) must be an array with valid callbacks as values, function "nonexistent" not found or invalid function name
XSLTProcessor::registerPHPFunctions(): Argument #1 ($functions) array key must not be empty
