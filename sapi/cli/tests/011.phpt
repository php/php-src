--TEST--
syntax check
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

$php = $_ENV['TEST_PHP_EXECUTABLE'];

$filename = dirname(__FILE__)."/011.test.php";

$code = '
<?php

$test = "var";

class test {
	private $var;
}

echo test::$var;

?>
';

file_put_contents($filename, $code);

var_dump(`"$php" -l "$filename"`);
var_dump(`"$php" -l some.unknown`);

$code = '
<?php

class test 
	private $var;
}

?>
';

file_put_contents($filename, $code);

var_dump(`"$php" -l "$filename"`);

@unlink($filename);

echo "Done\n";
?>
--EXPECTF--	
string(%d) "No syntax errors detected in %s011.test.php
"
string(40) "Could not open input file: some.unknown
"
string(%d) "
Parse error: syntax error, unexpected T_PRIVATE, expecting '{' in %s on line %d
Errors parsing %s011.test.php
"
Done
