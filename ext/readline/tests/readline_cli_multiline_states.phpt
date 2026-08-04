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
fwrite($pipes[0], "echo strtoupper('single \\\\ escape\n');\n");
fwrite($pipes[0], "echo strtoupper(\"double \\\\ escape\n\");\n");
fwrite($pipes[0], "echo strtoupper(\n\"paren\"\n);\n");
fwrite($pipes[0], "echo \"arithmetic=\", 6 /\n2 + 100;\n");
fwrite($pipes[0], "if (true) {\necho strtoupper(\"block\n\");\n}\n");
fwrite($pipes[0], "// line comment\n");
fwrite($pipes[0], "/*\n*/\necho strtoupper(\"comment\n\");\n");
fwrite($pipes[0], "#[AllowDynamicProperties]\nclass ReadlineCliCoverageClass {}\n");
fwrite($pipes[0], "echo strtoupper(\"attribute\n\");\n");
fwrite($pipes[0], "if (true) ?>out<?php\n?>side-\n<?php\n{\necho strtoupper(\"inside\n\");\n}\n");
fwrite($pipes[0], "quit\n");
fclose($pipes[0]);
proc_close($proc);
?>
--EXPECTF--
%AInteractive shell%ASINGLE%ADOUBLE%APAREN%Aarithmetic=103%ABLOCK%ACOMMENT%AATTRIBUTE%Aoutside-%AINSIDE%A
