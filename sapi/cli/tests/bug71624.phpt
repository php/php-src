--TEST--
Bug #61977 Test that -R properly sets argi and argn
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE');

$filename_txt = dirname(__FILE__) . DIRECTORY_SEPARATOR . "bug71624.test.txt";

$txt = 'foo
test
hello
';

file_put_contents($filename_txt, $txt);

$test_args = ['$argi', '$argn'];
foreach ($test_args as $test_arg) {
	if (substr(PHP_OS, 0, 3) == 'WIN') {
		var_dump(`type "$filename_txt" | "$php" -n -R "echo $test_arg . PHP_EOL;"`);
	} else {
		var_dump(`cat "$filename_txt" | "$php" -n -R 'echo $test_arg . PHP_EOL;'`);
	}
}

@unlink($filename_txt);

echo "Done\n";
?>
--EXPECT--
string(6) "1
2
3
"
string(15) "foo
test
hello
"
Done
