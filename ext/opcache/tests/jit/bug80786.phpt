--TEST--
Bug #80786: PHP crash using JIT
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=1M
opcache.jit=function
--EXTENSIONS--
opcache
--FILE--
<?php

$a = new Test();
$a->TestFunc();
var_dump($a->value);

class Test{
	public $value = 11.3;

	public function TestFunc() {
		$this->value -= 10;
	}
}

?>
--EXPECT--
float(1.3000000000000007)
