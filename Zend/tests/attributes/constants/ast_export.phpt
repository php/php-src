--TEST--
AST can be recreated when constants have attributes
--EXTENSIONS--
zend_test
--FILE--
<?php

#[MyAttrib]
const WITH_ATTRIBUTE = true;

#[First]
#[Second]
const WITH_UNGROUPED = true;

#[First, Second]
const WITH_GROUPED = true;

#[MyAttrib(5, param: "example")]
const WITH_PARAMETERS = true;

echo zend_test_compile_to_ast(file_get_contents(__FILE__));

?>
--EXPECT--
#[MyAttrib]
const WITH_ATTRIBUTE = true;
#[First]
#[Second]
const WITH_UNGROUPED = true;
#[First, Second]
const WITH_GROUPED = true;
#[MyAttrib(5, param: 'example')]
const WITH_PARAMETERS = true;
echo zend_test_compile_to_ast(file_get_contents(__FILE__));
