--TEST--
Configurable interactive results with libedit
--SKIPIF--
<?php
if (!extension_loaded('readline')) die('skip readline extension not available');
if (READLINE_LIB !== 'libedit') { die('skip libedit only'); }
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
fclose($pipes[0]);
proc_close($proc);
?>
--EXPECTF--
resource(%d) of type (process)
Interactive shell


int(2)
test

string(14) "php shell code"

string(7) "MyClass"

object(Closure)#%d (0) {
}
