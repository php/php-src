--TEST--
AST export of static member names that do not lex as identifiers
--EXTENSIONS--
zend_test
--FILE--
<?php

$code = <<<'PHP'
<?php
C::{'__halt_compiler'};
C::{'__HALT_compiler'};
$o::{'__halt_compiler'};
C::{'__halt_compiler'}();
C::{'__HALT_compiler'}();
$o::{'__halt_compiler'}();
C::{'a b'}();
C::{''}();
C::public( set );
C::protected( set );
C::private( set );
C::PUBLIC( SET );
$o::public( set );
C::{'public'}(set);
C::public();
C::static(set);
$o->__halt_compiler();
$o->public(set);
C::$__halt_compiler;
PHP;

$export = zend_test_compile_to_ast($code);
echo $export;
eval("return;\n$export");

?>
--EXPECT--
C::{'__halt_compiler'};
C::{'__HALT_compiler'};
$o::{'__halt_compiler'};
C::{'__halt_compiler'}();
C::{'__HALT_compiler'}();
$o::{'__halt_compiler'}();
C::{'a b'}();
C::{''}();
C::{'public'}(set);
C::{'protected'}(set);
C::{'private'}(set);
C::{'PUBLIC'}(SET);
$o::{'public'}(set);
C::{'public'}(set);
C::{'public'}();
C::static(set);
$o->__halt_compiler();
$o->public(set);
C::$__halt_compiler;
