--TEST--
Interactive shell: multiline input states
--EXTENSIONS--
readline
--SKIPIF--
<?php
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
$ini = getenv('TEST_PHP_EXTRA_ARGS');
$descriptorspec = [['pipe', 'r'], STDOUT, STDERR];
$proc = proc_open("$php $ini -a", $descriptorspec, $pipes);

fwrite($pipes[0], "\n");
fwrite($pipes[0], "# comment without ini assignment\n");
fwrite($pipes[0], "echo 'single\n';\n");
fwrite($pipes[0], "echo \"double\n\";\n");
fwrite($pipes[0], "echo strtoupper(\n\"paren\"\n);\n");
fwrite($pipes[0], "if (true) {\necho \"block\n\";\n}\n");
fwrite($pipes[0], "/*\n*/\necho \"comment\n\";\n");
fwrite($pipes[0], "#[AllowDynamicProperties]\nclass ReadlineCliCoverageClass {}\n");
fwrite($pipes[0], "echo \"attribute\n\";\n");
fwrite($pipes[0], "?>outside-<?php\n");
fwrite($pipes[0], "echo \"inside\n\";\n");
fwrite($pipes[0], "quit\n");
fclose($pipes[0]);
proc_close($proc);
?>
--EXPECTF--
%AInteractive shell%Asingle%Adouble%APAREN%Ablock%Acomment%Aattribute%Aoutside-%Ainside%A
