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
fwrite($pipes[0], "echo strtoupper('single \\' semi ; brace }\nend');\n");
fwrite($pipes[0], "echo strtoupper(\"double \\\" semi ; brace }\nend\");\n");
fwrite($pipes[0], "echo strtoupper(\n\"paren\"\n);\n");
fwrite($pipes[0], "echo \"arithmetic=\", 6 /\n2 + 100;\n");
fwrite($pipes[0], "if (true) {\necho strtoupper(\"block_body\n\");\n}\n");
fwrite($pipes[0], "echo strtoupper(\n\"hash_comment\\n\"\n# ) ;\n);\n");
fwrite($pipes[0], "echo strtoupper(\n\"slash_comment\\n\"\n// ) ;\n);\n");
fwrite($pipes[0], "echo strtoupper(\n\"block_comment\\n\" /*\n) ;\n*/\n);\n");
fwrite($pipes[0], "#[AllowDynamicProperties]\nclass ReadlineCliCoverageClass {}\n");
fwrite($pipes[0], "echo strtoupper((new ReflectionClass(ReadlineCliCoverageClass::class))->getAttributes()[0]->getName()), \"\\n\";\n");
fwrite($pipes[0], "if (true) ?>outside } );\n<?php\necho strtoupper(\"outside_ok\\n\");\n");
fwrite($pipes[0], "quit\n");
fclose($pipes[0]);
proc_close($proc);
?>
--EXPECTF--
%AInteractive shell%ASINGLE%ADOUBLE%APAREN%Aarithmetic=103%ABLOCK_BODY%AHASH_COMMENT%ASLASH_COMMENT%ABLOCK_COMMENT%AALLOWDYNAMICPROPERTIES%AOUTSIDE_OK%A
