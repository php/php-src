--TEST--
GH-18714 (opcache_compile_file() breaks class hoisting)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php

// Class used before declaration relies on hoisting
file_put_contents(__DIR__ . '/gh18714_test.php', <<<'PHP'
<?php
$x = new HelloWorld();
echo get_class($x) . "\n";
class HelloWorld {}
PHP);

opcache_compile_file(__DIR__ . '/gh18714_test.php');
require_once __DIR__ . '/gh18714_test.php';

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh18714_test.php');
?>
--EXPECT--
HelloWorld
