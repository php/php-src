--TEST--
registerPHPFunctionsNS() function
--EXTENSIONS--
xsl
--FILE--
<?php

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

echo "--- Legit cases: string callable ---\n";

$proc = createProcessor(["foo:var_dump(string(@href))"]);
$proc->registerPHPFunctionsNS('urn:foo', 'var_dump');
$proc->transformToXml($inputdom);

echo "--- Legit cases: string callable in array ---\n";

$proc = createProcessor(["foo:var_dump(string(@href))"]);
$proc->registerPHPFunctionsNS('urn:foo', ['var_dump']);
$proc->transformToXml($inputdom);

echo "--- Legit cases: callable in array ---\n";

$proc = createProcessor(["foo:test(string(@href))"]);
$proc->registerPHPFunctionsNS('urn:foo', ['test' => var_dump(...)]);
$proc->transformToXml($inputdom);

echo "--- Legit cases: multiple namespaces ---\n";

$proc = createProcessor(["foo:test(string(@href))", "bar:test(string(@href))"]);
$proc->registerPHPFunctionsNS('urn:foo', ['test' => strrev(...)]);
$proc->registerPHPFunctionsNS('urn:bar', ['test' => strtoupper(...)]);
var_dump($proc->transformToXml($inputdom));

echo "--- Error cases ---\n";

try {
    createProcessor([])->registerPhpFunctionsNS('http://php.net/xsl', ['strtolower']);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    createProcessor([])->registerPhpFunctionsNS('urn:foo', ['x:a' => 'strtolower']);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    createProcessor([])->registerPhpFunctionsNS("urn:foo", ["\0" => 'strtolower']);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    createProcessor([])->registerPhpFunctionsNS("\0", ['strtolower']);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    createProcessor([])->registerPhpFunctionsNS("urn:foo", [var_dump(...)]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
--- Legit cases: none ---

Warning: XSLTProcessor::transformToXml(): xmlXPathCompOpEval: function var_dump not found in %s on line %d

Warning: XSLTProcessor::transformToXml(): Unregistered function in %s on line %d

Warning: XSLTProcessor::transformToXml(): runtime error: file %s line 6 element value-of in %s on line %d

Warning: XSLTProcessor::transformToXml(): XPath evaluation returned no result. in %s on line %d
--- Legit cases: string callable ---
string(15) "https://php.net"
--- Legit cases: string callable in array ---
string(15) "https://php.net"
--- Legit cases: callable in array ---
string(15) "https://php.net"
--- Legit cases: multiple namespaces ---
string(53) "<?xml version="1.0"?>
ten.php//:sptthHTTPS://PHP.NET
"
--- Error cases ---
XSLTProcessor::registerPHPFunctionsNS(): Argument #1 ($namespace) must not be "http://php.net/xsl" because it is reserved for PHP
XSLTProcessor::registerPHPFunctionsNS(): Argument #1 ($namespace) must be an array containing valid callback names
XSLTProcessor::registerPHPFunctionsNS(): Argument #1 ($namespace) must be an array containing valid callback names
XSLTProcessor::registerPHPFunctionsNS(): Argument #1 ($namespace) must not contain any null bytes
Object of class Closure could not be converted to string
