--TEST--
PRE_INC_OBJ: 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.protect_memory=1
--FILE--
<?php
class Test {
    public $prop;
	function foo() {
		$this->prop = PHP_INT_MAX - 5;
		for ($i = 0; $i < 10; $i++) {
			var_dump(++$this->prop);
		}
	}
}

$test = new Test;
$test->foo();
?>
--EXPECTF--
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
float(%f)
float(%f)
float(%f)
float(%f)
float(%f)
