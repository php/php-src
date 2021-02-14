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
    function(string $code, $result) {
        if (isset($result)) {
            echo "\n";
            var_dump($result);
        }});

EOT);
fwrite($pipes[0], "1+1;\n");
fwrite($pipes[0], 'echo "test\n";' . "\n");
fwrite($pipes[0], "__FILE__;\n");
fwrite($pipes[0], "namespace\MyClass::class;\n");
fwrite($pipes[0], "fn()=>true;");
// Setting the callback to null clears the handler.
fwrite($pipes[0], "readline_interactive_shell_result_function(null);\n");
fwrite($pipes[0], "1+1;\n");
// This passes (string $code, $result) to any callable
fwrite($pipes[0], "readline_interactive_shell_result_function('var_dump');\n");
fwrite($pipes[0], "2+2;\n");
fclose($pipes[0]);
proc_close($proc);
?>
--EXPECTF--
resource(%d) of type (process)
Interactive shell

php > readline_interactive_shell_result_function(
php (     function(string $code, $result) {
php (         if (isset($result)) {
php (             echo "\n";
php (             var_dump($result);
php (         }});

bool(true)
php > 1+1;

int(2)
php > echo "test\n";
test
php > __FILE__;

string(14) "php shell code"
php > namespace\MyClass::class;

string(7) "MyClass"
php > fn()=>true;readline_interactive_shell_result_function(null);
php > 1+1;
php > readline_interactive_shell_result_function('var_dump');
php > 2+2;
string(5) "2+2;
"
int(4)
php >
