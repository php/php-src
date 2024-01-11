--TEST--
registerPHPFunctionNS() function
--EXTENSIONS--
xsl
--FILE--
<?php

class TrampolineClass {
    public static function __callStatic(string $name, array $arguments): mixed {
        var_dump($name, $arguments);
        return "foo";
    }
}

class StatefulClass {
    public array $state = [];

    public function __call(string $name, array $arguments): mixed {
        $this->state[] = [$name, $arguments[0]];
        return $arguments[0];
    }
}

function createProcessor($inputs) {
    $xsl = new DomDocument();
    $xsl->loadXML('<?xml version="1.0" encoding="iso-8859-1" ?>
    <xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:foo="urn:foo"
    xmlns:bar="urn:bar">
    <xsl:template match="//a">'
    . implode('', array_map(fn($input) => '<xsl:value-of select="' . $input . '" />', $inputs)) .
    '</xsl:template>
    </xsl:stylesheet>');

    $proc = new XSLTProcessor();
    $proc->importStylesheet($xsl);
    return $proc;
}

$inputdom = new DomDocument();
$inputdom->loadXML('<?xml version="1.0" encoding="iso-8859-1"?><a href="https://php.net">hello</a>');

echo "--- Legit cases: none ---\n";

$proc = createProcessor(["foo:var_dump(string(@href))"]);
try {
    $proc->transformToXml($inputdom);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

echo "--- Legit cases: global function callable ---\n";

$proc = createProcessor(["foo:var_dump(string(@href))"]);
$proc->registerPHPFunctionNS('urn:foo', 'var_dump', var_dump(...));
$proc->transformToXml($inputdom);

echo "--- Legit cases: global string callable ---\n";

$proc = createProcessor(["foo:var_dump(string(@href))"]);
$proc->registerPHPFunctionNS('urn:foo', 'var_dump', 'var_dump');
$proc->transformToXml($inputdom);

echo "--- Legit cases: trampoline callable ---\n";

$proc = createProcessor(["foo:trampoline(string(@href))"]);
$proc->registerPHPFunctionNS('urn:foo', 'trampoline', TrampolineClass::test(...));
var_dump($proc->transformToXml($inputdom));

echo "--- Legit cases: instance class method callable ---\n";

$state = new StatefulClass;
$proc = createProcessor(["foo:test(string(@href))"]);
$proc->registerPHPFunctionNS('urn:foo', 'test', $state->test(...));
var_dump($proc->transformToXml($inputdom));
var_dump($state->state);

echo "--- Legit cases: multiple namespaces ---\n";

$proc = createProcessor(["foo:test(string(@href))", "bar:test(string(@href))"]);
$proc->registerPHPFunctionNS('urn:foo', 'test', strrev(...));
$proc->registerPHPFunctionNS('urn:bar', 'test', strtoupper(...));
var_dump($proc->transformToXml($inputdom));

echo "--- Error cases ---\n";

try {
    createProcessor([])->registerPhpFunctionNS('http://php.net/xsl', 'strtolower', strtolower(...));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    createProcessor([])->registerPhpFunctionNS('urn:foo', 'x:a', strtolower(...));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    createProcessor([])->registerPhpFunctionNS("urn:foo", "\0", strtolower(...));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    createProcessor([])->registerPhpFunctionNS("\0", 'strtolower', strtolower(...));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
--- Legit cases: none ---

Warning: XSLTProcessor::transformToXml(): xmlXPathCompOpEval: function var_dump not found in %s on line %d

Warning: XSLTProcessor::transformToXml(): Unregistered function in %s on line %d

Warning: XSLTProcessor::transformToXml(): runtime error: file %s line 6 element value-of in %s on line %d

Warning: XSLTProcessor::transformToXml(): XPath evaluation returned no result. in %s on line %d
--- Legit cases: global function callable ---
string(15) "https://php.net"
--- Legit cases: global string callable ---
string(15) "https://php.net"
--- Legit cases: trampoline callable ---
string(4) "test"
array(1) {
  [0]=>
  string(15) "https://php.net"
}
string(26) "<?xml version="1.0"?>
foo
"
--- Legit cases: instance class method callable ---
string(38) "<?xml version="1.0"?>
https://php.net
"
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(4) "test"
    [1]=>
    string(15) "https://php.net"
  }
}
--- Legit cases: multiple namespaces ---
string(53) "<?xml version="1.0"?>
ten.php//:sptthHTTPS://PHP.NET
"
--- Error cases ---
XSLTProcessor::registerPHPFunctionNS(): Argument #1 ($namespaceURI) must not be "http://php.net/xsl" because it is reserved by PHP
XSLTProcessor::registerPHPFunctionNS(): Argument #2 ($name) must be a valid callback name
XSLTProcessor::registerPHPFunctionNS(): Argument #2 ($name) must not contain any null bytes
XSLTProcessor::registerPHPFunctionNS(): Argument #1 ($namespaceURI) must not contain any null bytes
