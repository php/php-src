--TEST--
Zend: Test compile string
--EXTENSIONS--
zend_test
--FILE--
<?php

define('ZEND_COMPILE_POSITION_AT_SHEBANG', 0);
define('ZEND_COMPILE_POSITION_AT_OPEN_TAG', 1);
define('ZEND_COMPILE_POSITION_AFTER_OPEN_TAG', 2);

$source_string = <<<EOF
#!/path/to/php
<?php
var_dump('php');
EOF;

zend_test_compile_string($source_string, 'Source string', ZEND_COMPILE_POSITION_AT_SHEBANG);

$source_string = <<<EOF
#!/path/to/php
<?php
var_dump('php');
EOF;

zend_test_compile_string($source_string, 'Source string', ZEND_COMPILE_POSITION_AT_OPEN_TAG);

$source_string = <<<EOF
<?php
var_dump('php');
EOF;

zend_test_compile_string($source_string, 'Source string', ZEND_COMPILE_POSITION_AT_OPEN_TAG);

$source_string = <<<EOF
var_dump('php');
EOF;

zend_test_compile_string($source_string, 'Source string', ZEND_COMPILE_POSITION_AFTER_OPEN_TAG);

$source_string = <<<EOF
<?php
var_dump('php');
EOF;

try {
	zend_test_compile_string($source_string, "Sources\0string", ZEND_COMPILE_POSITION_AT_OPEN_TAG);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}

zend_test_compile_string(null, null, null);

$source_string = <<<EOF
<?php
var_dump('php');
EOF;

zend_test_compile_string($source_string, 'Source string', ZEND_COMPILE_POSITION_AFTER_OPEN_TAG);

?>
--EXPECTF--
string(3) "php"
#!/path/to/php
string(3) "php"
string(3) "php"
string(3) "php"
zend_test_compile_string(): Argument #2 ($filename) must not contain any null bytes

Deprecated: zend_test_compile_string(): Passing null to parameter #1 ($source_string) of type string is deprecated in %s on line %d

Deprecated: zend_test_compile_string(): Passing null to parameter #2 ($filename) of type string is deprecated in %s on line %d

Deprecated: zend_test_compile_string(): Passing null to parameter #3 ($position) of type int is deprecated in %s on line %d

Parse error: syntax error, unexpected token "<", expecting end of file in Source string on line 1
