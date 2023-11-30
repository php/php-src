--TEST--
registerPhpFunctionsNS() function
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument();
$doc->loadHTML('<a href="https://PHP.net">hello</a>');

$xpath = new DOMXPath($doc);

echo "--- Error cases ---\n";

try {
    $xpath->registerPhpFunctionsNS('http://php.net/xpath', ['strtolower']);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $xpath->registerPhpFunctionsNS('urn:foo', ['x:a' => 'strtolower']);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $xpath->registerPhpFunctionsNS("urn:foo", ["\0" => 'strtolower']);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $xpath->registerPhpFunctionsNS("\0", ['strtolower']);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $xpath->registerPhpFunctionsNS("urn:foo", [var_dump(...)]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

echo "--- Legit cases: string callable ---\n";

$xpath->registerNamespace('foo', 'urn:foo');
$xpath->registerPhpFunctionsNS('urn:foo', 'strtolower');
var_dump($xpath->query('//a[foo:strtolower(string(@href)) = "https://php.net"]'));

echo "--- Legit cases: string callable in array ---\n";

$xpath->registerPhpFunctionsNS('urn:foo', ['strtoupper']);
var_dump($xpath->query('//a[foo:strtoupper(string(@href)) = "https://php.net"]'));

echo "--- Legit cases: callable in array ---\n";

$xpath->registerPhpFunctionsNS('urn:foo', ['test' => 'var_dump']);
$xpath->query('//a[foo:test(string(@href))]');

echo "--- Legit cases: multiple namespaces ---\n";

$xpath->registerNamespace('bar', 'urn:bar');
$xpath->registerPhpFunctionsNS('urn:bar', ['test' => 'strtolower']);
var_dump($xpath->query('//a[bar:test(string(@href)) = "https://php.net"]'));

?>
--EXPECT--
--- Error cases ---
DOMXPath::registerPhpFunctionsNS(): Argument #1 ($namespace) must not be "http://php.net/xpath" because it is reserved for PHP
DOMXPath::registerPhpFunctionsNS(): Argument #1 ($namespace) must be an array containing valid callback names
DOMXPath::registerPhpFunctionsNS(): Argument #1 ($namespace) must be an array containing valid callback names
DOMXPath::registerPhpFunctionsNS(): Argument #1 ($namespace) must not contain any null bytes
Object of class Closure could not be converted to string
--- Legit cases: string callable ---
object(DOMNodeList)#6 (1) {
  ["length"]=>
  int(1)
}
--- Legit cases: string callable in array ---
object(DOMNodeList)#6 (1) {
  ["length"]=>
  int(0)
}
--- Legit cases: callable in array ---
string(15) "https://PHP.net"
--- Legit cases: multiple namespaces ---
object(DOMNodeList)#4 (1) {
  ["length"]=>
  int(1)
}
