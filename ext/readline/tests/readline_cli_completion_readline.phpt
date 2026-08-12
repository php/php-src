--TEST--
Interactive shell: default completion function
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

$code = <<<'PHP'
$readline_cli_completion_variable = strtolower("VARIABLE_OK\n");
echo $readline_cli_completion_var	;
#prec	3
echo "precision_result=" . ini_get("precision") . "\n";
function readline_cli_completion_function() { echo strtolower("FUNCTION_OK\n"); }
readline_cli_completion_fun	);
define('READLINE_CLI_COMPLETION_CONSTANT', strtolower("CONSTANT_OK\n"));
echo READLINE_CLI_COMPLETION_CON	;
class ReadlineCliCompletionClass {
    public static function completionMethod() { echo strtolower("METHOD_OK\n"); }
}
echo "class_ok:" . ReadlineCliCompletionCla	::class . "\n";
ReadlineCliCompletionClass::completionM	);
exit
PHP;

echo shell_exec("echo " . escapeshellarg($code) . " | $php $ini -a");
?>
--EXPECTF--
%AInteractive shell%Avariable_ok%Aprecision_result=3%Afunction_ok%Aconstant_ok%Aclass_ok:ReadlineCliCompletionClass%Amethod_ok%A
