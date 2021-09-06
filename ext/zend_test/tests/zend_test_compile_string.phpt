--TEST--
Zend: Test compile string
--EXTENSIONS--
zend_test
--FILE--
<?php
$source_string = <<<EOF
<?php
var_dump('php');
EOF;

zend_test_compile_string($source_string, 'Source string', 1);

$source_string = <<<EOF
var_dump('php');
EOF;

zend_test_compile_string($source_string, 'Source string', 0);

$source_string = <<<EOF
<?php
var_dump('php');
EOF;

zend_test_compile_string($source_string, 'Source string', 0);
?>
--EXPECT--
string(3) "php"
string(3) "php"

Parse error: syntax error, unexpected token "<", expecting end of file in Source string on line 1
