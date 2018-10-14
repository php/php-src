--TEST--
execute a file with -f
--SKIPIF--
<?php
include "skipif.inc";
if (substr(PHP_OS, 0, 3) == 'WIN') {
	die ("skip not for Windows");
}
?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE');

$filename = dirname(__FILE__).'/008.test.php';
$code ='
<?php

class test { 
	private $pri; 
}

var_dump(test::$pri);
?>
';

file_put_contents($filename, $code);

var_dump(`$php -n -f "$filename"`);
var_dump(`$php -n -f "wrong"`);

@unlink($filename);

echo "Done\n";
?>
--EXPECTF--
string(%d) "

Fatal error: Uncaught Error: Cannot access private property test::$pri in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
"
string(33) "Could not open input file: wrong
"
Done
