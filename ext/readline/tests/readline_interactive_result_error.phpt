--TEST--
Configurable interactive results in readline
--INI--
cli.enable_interactive_shell_result_function=1
--SKIPIF--
<?php
if (!extension_loaded('readline')) die('skip readline extension not available');
if (READLINE_LIB !== 'readline') { die('skip readline only'); }
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE');
$ini = getenv('TEST_PHP_EXTRA_ARGS');
$descriptorspec = [['pipe', 'r'], STDOUT, STDERR];
$proc = proc_open("$php $ini -a", $descriptorspec, $pipes);
var_dump($proc);

// var_dump, json_encode, or any more complex dumping can be used in readline_interactive_shell_result_function.
fwrite($pipes[0], <<<'EOT'
readline_interactive_shell_result_function(
    function(string $code, $result, $unexpectedExtraParam) {
        if (isset($result)) {
            echo "\n";
            var_dump($result);
        }});

EOT);
fwrite($pipes[0], "sprintf('hello, %s', 'world');;\n");
fwrite($pipes[0], 'for($i = 0; $i < 10; $i++) { echo $i; }; echo "\n";' . "\n");
fwrite($pipes[0], "fn()=>true;");
fclose($pipes[0]);
proc_close($proc);
?>
--EXPECTF--
resource(%d) of type (process)
Interactive shell

php > readline_interactive_shell_result_function(
php (     function(string $code, $result, $unexpectedExtraParam) {
php (         if (isset($result)) {
php (             echo "\n";
php (             var_dump($result);
php (         }});
php > sprintf('hello, %s', 'world');;

Fatal error: Uncaught ArgumentCountError: Too few arguments to function {closure}(), 2 passed and exactly 3 expected in php shell code:2
Stack trace:
#0 [internal function]: {closure}('sprintf('hello,...', 'hello, world')
#1 {main}
  thrown in php shell code on line 2
php > for($i = 0; $i < 10; $i++) { echo $i; }; echo "\n";
0123456789
php > fn()=>true;

Fatal error: Uncaught ArgumentCountError: Too few arguments to function {closure}(), 2 passed and exactly 3 expected in php shell code:2
Stack trace:
#0 [internal function]: {closure}('fn()=>true;\n', Object(Closure))
#1 {main}
  thrown in php shell code on line 2
php >
