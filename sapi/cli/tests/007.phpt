--TEST--
strip comments and whitespace with -w
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

$filename = __DIR__.'/007.test.php';
$code ='
<?php
/* some test script */

class test { /* {{{ */
	public $var = "test"; //test var
#perl style comment 
	private $pri; /* private attr */

	function foo(/* void */) {
	}
}
/* }}} */

?>
';

file_put_contents($filename, $code);

var_dump(`$php -n -w "$filename"`);
var_dump(`$php -n -w "wrong"`);
var_dump(`echo "<?php /* comment */ class test {\n // comment \n function foo() {} } ?>" | $php -n -w`);

@unlink($filename);

echo "Done\n";
?>
--EXPECT--
string(81) "
<?php
 class test { public $var = "test"; private $pri; function foo() { } } ?>
"
string(33) "Could not open input file: wrong
"
string(43) "<?php  class test { function foo() {} } ?>
"
Done
