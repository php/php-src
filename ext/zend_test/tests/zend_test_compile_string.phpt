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

zend_test_compile_string($source_string, 'Source string', ZEND_COMPILE_POSITION_AFTER_OPEN_TAG);
?>
--EXPECT--
Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0

Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0
string(3) "php"
#!/path/to/php
string(3) "php"
string(3) "php"
string(3) "php"

Parse error: syntax error, unexpected token "<", expecting end of file in Source string on line 1
