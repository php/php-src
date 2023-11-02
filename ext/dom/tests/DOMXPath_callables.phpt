--TEST--
registerPHPFunctions() with callables
--EXTENSIONS--
dom
--FILE--
<?php

class MyClass {
    public static function dump(string $var) {
        var_dump($var);
    }
}

class MyDOMXPath extends DOMXPath {
    public function registerCycle() {
        $this->registerPhpFunctions(["cycle" => array($this, "dummy")]);
    }

    public function dummy(string $var) {
        echo "dummy: $var\n";
    }
}

$doc = new DOMDocument();
$doc->loadHTML('<a href="https://php.net">hello</a>');

echo "--- Legit cases: none ---\n";

$xpath = new DOMXPath($doc);
$xpath->registerNamespace("php", "http://php.net/xpath");
try {
    $xpath->evaluate("//a[php:function('var_dump', string(@href))]");
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

echo "--- Legit cases: all ---\n";

$xpath->registerPHPFunctions(null);
$xpath->evaluate("//a[php:function('var_dump', string(@href))]");
$xpath->evaluate("//a[php:function('MyClass::dump', string(@href))]");

echo "--- Legit cases: set ---\n";

$xpath = new DOMXPath($doc);
$xpath->registerNamespace("php", "http://php.net/xpath");
$xpath->registerPhpFunctions([]);
$xpath->registerPHPFunctions(["xyz" => MyClass::dump(...), "mydump" => function (string $x) {
    var_dump($x);
}]);
$xpath->registerPhpFunctions(str_repeat("var_dump", mt_rand(1, 1) /* defeat SCCP */));
$xpath->evaluate("//a[php:function('mydump', string(@href))]");
$xpath->evaluate("//a[php:function('xyz', string(@href))]");
$xpath->evaluate("//a[php:function('var_dump', string(@href))]");
try {
    $xpath->evaluate("//a[php:function('notinset', string(@href))]");
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

echo "--- Legit cases: set with cycle ---\n";

$xpath = new MyDOMXPath($doc);
$xpath->registerNamespace("php", "http://php.net/xpath");
$xpath->registerCycle();
$xpath->evaluate("//a[php:function('cycle', string(@href))]");

echo "--- Error cases ---\n";

$xpath = new DOMXPath($doc);
try {
    $xpath->registerPhpFunctions("nonexistent");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $xpath->registerPhpFunctions(function () {});
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $xpath->registerPhpFunctions([function () {}]);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

try {
    $xpath->registerPhpFunctions([var_dump(...)]);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

try {
    $xpath->registerPhpFunctions(["nonexistent"]);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

try {
    $xpath->registerPhpFunctions(["" => var_dump(...)]);
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
--- Legit cases: set ---
string(15) "https://php.net"
string(15) "https://php.net"
string(15) "https://php.net"
No callback handler "notinset" registered
--- Legit cases: set with cycle ---
dummy: https://php.net
--- Error cases ---
DOMXPath::registerPhpFunctions(): Argument #1 ($restrict) must be a callable, function "nonexistent" not found or invalid function name
DOMXPath::registerPhpFunctions(): Argument #1 ($restrict) must be of type array|string|null, Closure given
Object of class Closure could not be converted to string
Object of class Closure could not be converted to string
DOMXPath::registerPhpFunctions(): Argument #1 ($restrict) must be an array with valid callbacks as values, function "nonexistent" not found or invalid function name
DOMXPath::registerPhpFunctions(): Argument #1 ($restrict) array key must not be empty
