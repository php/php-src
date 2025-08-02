--TEST--
Bug #75218: Change remaining uncatchable fatal errors for parsing into ParseError
--FILE--
<?php

function try_eval($code) {
    try {
        eval($code);
    } catch (CompileError $e) {
        echo $e->getMessage(), "\n";
    }
}

try_eval('if (false) {class C { final final function foo($fff) {}}}');
try_eval('if (false) {class C { private protected $x; }}');
try_eval('if (true) { __HALT_COMPILER(); }');
try_eval('declare(encoding=[]);');

?>
--EXPECT--
Multiple final modifiers are not allowed
Multiple access type modifiers are not allowed
__HALT_COMPILER() can only be used from the outermost scope
Encoding must be a literal
