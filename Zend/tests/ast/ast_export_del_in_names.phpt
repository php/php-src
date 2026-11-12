--TEST--
AST export of names containing 0x7f
--EXTENSIONS--
zend_test
--FILE--
<?php

$code = <<<'PHP'
<?php
C::{"\x7f"};
C::{"a\x7f"};
C::{"\x7fa"};
$o::{"\x7f"};
C::{"\x7f"}();
$o::{"\x7f"}();
${"\x7f"};
${"a\x7f"};
$o->{"\x7f"};
$o?->{"\x7f"};
$o->{"\x7f"}();
$o?->{"\x7f"}();
C::${"\x7f"};
"x{${"\x7f"}}y";
"x{$o->{"\x7f"}}y";
"{$a}\x7f";
C::{"\x80"};
${"\xff"};
$o->{"a\x80"};
"{$a}\x80";
PHP;

$export = zend_test_compile_to_ast($code);
echo addcslashes($export, "\x7f..\xff");
eval("return;\n$export");

?>
--EXPECT--
C::{'\177'};
C::{'a\177'};
C::{'\177a'};
$o::{'\177'};
C::{'\177'}();
$o::{'\177'}();
${'\177'};
${'a\177'};
$o->{'\177'};
$o?->{'\177'};
$o->{'\177'}();
$o?->{'\177'}();
C::${'\177'};
"x{${'\177'}}y";
"x{$o->{'\177'}}y";
"{$a}\177";
C::\200;
$\377;
$o->a\200;
"{$a}\200";
