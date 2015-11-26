--TEST--
php_strip_whitespace() tests
--FILE--
<?php

$filename = dirname(__FILE__)."/php_strip_whitespace.dat";

var_dump(php_strip_whitespace());
var_dump(php_strip_whitespace($filename));

$data = '/* test comment */';
file_put_contents($filename, $data);
var_dump(php_strip_whitespace($filename));

$data = '<?php /* test comment */ ?>';
file_put_contents($filename, $data);
var_dump(php_strip_whitespace($filename));

$data = '<?php
/* test class */
class test {

	/* function foo () */

	function foo () /* {{{ */
	{


		echo $var; //does not exist
	}
	/* }}} */

}
?>';

file_put_contents($filename, $data);
var_dump(php_strip_whitespace($filename));

@unlink($filename);
echo "Done\n";
?>
--EXPECTF--	
Warning: php_strip_whitespace() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)

Warning: php_strip_whitespace(%sphp_strip_whitespace.dat): failed to open stream: No such file or directory in %s on line %d
string(0) ""
string(18) "/* test comment */"
string(9) "<?php  ?>"
string(55) "<?php
 class test { function foo () { echo $var; } } ?>"
Done
