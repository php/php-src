--TEST--
registerPhpFunctionNS() function
--EXTENSIONS--
dom
--FILE--
<?php

class TrampolineClass {
    public static function __callStatic(string $name, array $arguments): mixed {
        var_dump($name, $arguments);
        return strtoupper($arguments[0]);
    }
}

class StatefulClass {
    public array $state = [];

    public function __call(string $name, array $arguments): mixed {
        $this->state[] = [$name, $arguments[0]];
        return $arguments[0];
    }
}

$doc = new DOMDocument();
$doc->loadHTML('<a href="https://PHP.net">hello</a>');

$xpath = new DOMXPath($doc);

echo "--- Error cases ---\n";

try {
    $xpath->registerPhpFunctionNS('http://php.net/xpath', 'strtolower', strtolower(...));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $xpath->registerPhpFunctionNS('urn:foo', 'x:a', strtolower(...));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $xpath->registerPhpFunctionNS("urn:foo", "\0", strtolower(...));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $xpath->registerPhpFunctionNS("\0", 'strtolower', strtolower(...));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

$xpath->registerNamespace('foo', 'urn:foo');

echo "--- Legit cases: global function callable ---\n";

$xpath->registerPhpFunctionNS('urn:foo', 'strtolower', strtolower(...));
var_dump($xpath->query('//a[foo:strtolower(string(@href)) = "https://php.net"]'));

echo "--- Legit cases: string callable ---\n";

$xpath->registerPhpFunctionNS('urn:foo', 'strtolower', 'strtolower');
var_dump($xpath->query('//a[foo:strtolower(string(@href)) = "https://php.net"]'));

echo "--- Legit cases: trampoline callable ---\n";

$xpath->registerPhpFunctionNS('urn:foo', 'test', TrampolineClass::test(...));
var_dump($xpath->query('//a[foo:test(string(@href)) = "https://php.net"]'));

echo "--- Legit cases: instance class method callable ---\n";

$state = new StatefulClass;
$xpath->registerPhpFunctionNS('urn:foo', 'test', $state->test(...));
var_dump($xpath->query('//a[foo:test(string(@href))]'));
var_dump($state->state);

echo "--- Legit cases: global function callable that returns nothing ---\n";

$xpath->registerPhpFunctionNS('urn:foo', 'test', var_dump(...));
$xpath->query('//a[foo:test(string(@href))]');

echo "--- Legit cases: multiple namespaces ---\n";

$xpath->registerNamespace('bar', 'urn:bar');
$xpath->registerPhpFunctionNS('urn:bar', 'test', 'strtolower');
var_dump($xpath->query('//a[bar:test(string(@href)) = "https://php.net"]'));

?>
--EXPECT--
--- Error cases ---
DOMXPath::registerPhpFunctionNS(): Argument #1 ($namespaceURI) must not be "http://php.net/xpath" because it is reserved for PHP
DOMXPath::registerPhpFunctionNS(): Argument #2 ($name) must be a valid callback name
DOMXPath::registerPhpFunctionNS(): Argument #2 ($name) must not contain any null bytes
DOMXPath::registerPhpFunctionNS(): Argument #1 ($namespaceURI) must not contain any null bytes
--- Legit cases: global function callable ---
object(DOMNodeList)#7 (1) {
  ["length"]=>
  int(1)
}
--- Legit cases: string callable ---
object(DOMNodeList)#7 (1) {
  ["length"]=>
  int(1)
}
--- Legit cases: trampoline callable ---
string(4) "test"
array(1) {
  [0]=>
  string(15) "https://PHP.net"
}
object(DOMNodeList)#3 (1) {
  ["length"]=>
  int(0)
}
--- Legit cases: instance class method callable ---
object(DOMNodeList)#8 (1) {
  ["length"]=>
  int(1)
}
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(4) "test"
    [1]=>
    string(15) "https://PHP.net"
  }
}
--- Legit cases: global function callable that returns nothing ---
string(15) "https://PHP.net"
--- Legit cases: multiple namespaces ---
object(DOMNodeList)#7 (1) {
  ["length"]=>
  int(1)
}
