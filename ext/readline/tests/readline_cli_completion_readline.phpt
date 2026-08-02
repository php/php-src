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
$readline_cli_completion_variable = "variable\n";
echo $readline_cli_completion_var	;
#prec	3
echo "precision=" . ini_get("precision") . "\n";
function readline_cli_completion_function() { echo "function\n"; }
readline_cli_completion_fun	);
define('READLINE_CLI_COMPLETION_CONSTANT', "constant\n");
echo READLINE_CLI_COMPLETION_CON	;
class ReadlineCliCompletionClass {
    public const COMPLETION_CLASS_CONSTANT = "class constant\n";
    public static function completionMethod() { echo "method\n"; }
}
echo ReadlineCliCompletionCla	::class . "\n";
echo ReadlineCliCompletionClass::COMPLETION_CLASS_CON	;
ReadlineCliCompletionClass::completionM	);
exit
PHP;

echo shell_exec("echo " . escapeshellarg($code) . " | $php $ini -a");
?>
--EXPECTF--
%AInteractive shell%Avariable%Aprecision=3%Afunction%Aconstant%AReadlineCliCompletionClass%Aclass constant%Amethod%A
