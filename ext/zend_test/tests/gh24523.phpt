--TEST--
GH-24523 (UAF in zend_test_compile_to_ast() on parse error)
--EXTENSIONS--
zend_test
--FILE--
<?php

try {
    zend_test_compile_to_ast("<?php (");
} catch (ParseError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Unclosed '('
