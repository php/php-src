--TEST--
Interactive shell: class constant completion
--EXTENSIONS--
readline
--SKIPIF--
<?php
if (READLINE_LIB !== "readline") die('skip readline only');
if (!function_exists('shell_exec')) die('skip shell_exec() not available');
?>
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
$ini = getenv('TEST_PHP_EXTRA_ARGS');

putenv('TERM=VT100');
putenv('INPUTRC=/dev/null');

$code = <<<'PHP'
class Foo {
    public const FooBar = "wrong_constant\n";
    public const Zed = "zed_ok\n";
}
echo strtoupper(Foo::Ze	);
exit
PHP;

echo shell_exec("echo " . escapeshellarg($code) . " | $php $ini -a");
?>
--EXPECTF--
%AInteractive shell%AZED_OK%A
