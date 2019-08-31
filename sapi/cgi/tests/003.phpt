--TEST--
strip comments and whitespace with -w
--SKIPIF--
<?php

if (substr(PHP_OS, 0, 3) == 'WIN') {
	die ("skip not for Windows");
}

include "skipif.inc";
?>
--FILE--
<?php

include "include.inc";

$php = get_cgi_path();
reset_env_vars();

$filename = __DIR__.'/003.test.php';
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
--EXPECTF--
string(%d) "X-Powered-By: PHP/%s
Content-type: text/html%r; charset=.*|%r


<?php
 class test { public $var = "test"; private $pri; function foo() { } } ?>
"
string(%d) "Status: 404 Not Found
X-Powered-By: PHP/%s
Content-type: text/html%r; charset=.*|%r

No input file specified.
"
string(%d) "X-Powered-By: PHP/%s
Content-type: text/html%r; charset=.*|%r

<?php  class test { function foo() {} } ?>
"
Done
