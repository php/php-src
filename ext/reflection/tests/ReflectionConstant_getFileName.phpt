--TEST--
ReflectionConstant::getFileName()
--FILE--
<?php

include "included5.inc";

function testConstant(string $name): void {
    $ref = new ReflectionConstant($name);
    echo "$name: ";
    var_dump($ref->getFileName());
}

define('IN_CURRENT_FILE_DEFINED', 42);
const IN_CURRENT_FILE_AST = 123;

echo "From PHP:\n";
testConstant('PHP_VERSION');
testConstant('STDIN');
testConstant('STDOUT');
testConstant('STDERR');

echo "\nFrom the current file:\n";
testConstant('IN_CURRENT_FILE_DEFINED');
testConstant('IN_CURRENT_FILE_AST');

echo "\nFrom an included file:\n";
testConstant('INCLUDED_CONSTANT_DEFINED');
testConstant('INCLUDED_CONSTANT_AST');
?>
--EXPECTF--
From PHP:
PHP_VERSION: bool(false)
STDIN: bool(false)
STDOUT: bool(false)
STDERR: bool(false)

From the current file:
IN_CURRENT_FILE_DEFINED: string(%d) "%sReflectionConstant_getFileName.php"
IN_CURRENT_FILE_AST: string(%d) "%sReflectionConstant_getFileName.php"

From an included file:
INCLUDED_CONSTANT_DEFINED: string(%d) "%sincluded5.inc"
INCLUDED_CONSTANT_AST: string(%d) "%sincluded5.inc"
