--TEST--
Bug #80839: PHP problem with JIT
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=64M
opcache.jit=function
--EXTENSIONS--
opcache
--FILE--
<?php
$a = null; // the problem only occurs when set to NULL
test($a, 'y');

function test($str, $pad) {
	$x = $str . str_repeat($pad, 15); // $x now contains "yyyyyyyyyyyyyyy"
	var_dump($x);

	$gft = new gft();
	$gft->info(33);

	// $x has been changed ????
	// $x contains what was echoed in the function 'info'
	var_dump($x);
}
class gft {
	private $strVal = 'abcd ';
	public function info($info, $prefix = ' Info:') {
		echo $this->strVal.$prefix.serialize($info).'aaaa';
		echo "\n";
	}
}
?>
--EXPECT--
string(15) "yyyyyyyyyyyyyyy"
abcd  Info:i:33;aaaa
string(15) "yyyyyyyyyyyyyyy"
