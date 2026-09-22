--TEST--
Interactive shell: custom prompt escape sequences
--EXTENSIONS--
readline
--SKIPIF--
<?php
if (READLINE_LIB !== "readline") die('skip readline only');
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
$ini = getenv('TEST_PHP_EXTRA_ARGS');
$prompt = 'pre\\\\-\n-\t-\e-\v-\b-\>-\`-\q-' . "\xC3\xA9\xC3\xA9" . '-`echo \'dyn\';`-`-x ';
$descriptorspec = [['pipe', 'r'], STDOUT, STDERR];
$proc = proc_open("$php $ini -d " . escapeshellarg("cli.prompt=\"$prompt\"") . " -a", $descriptorspec, $pipes);
fwrite($pipes[0], "if (true) {\n");
fwrite($pipes[0], "echo strtoupper(\"prompt_body\\n\");\n");
fwrite($pipes[0], "}\n");
fwrite($pipes[0], "quit\n");
fclose($pipes[0]);
proc_close($proc);
?>
--EXPECTF--
Interactive shell


Warning: prompt contains unsupported unicode characters in Unknown on line 0
dynpre\-
-	-%c-%d.%d.%s-php->-`-\q-????-dyn--x if (true) {

Warning: prompt contains unsupported unicode characters in Unknown on line 0
dynpre\-
-	-%c-%d.%d.%s-php-{-`-\q-????-dyn--x echo strtoupper("prompt_body\n");

Warning: prompt contains unsupported unicode characters in Unknown on line 0
dynpre\-
-	-%c-%d.%d.%s-php-{-`-\q-????-dyn--x }

Warning: prompt contains unsupported unicode characters in Unknown on line 0
dynPROMPT_BODY
pre\-
-	-%c-%d.%d.%s-php->-`-\q-????-dyn--x quit
