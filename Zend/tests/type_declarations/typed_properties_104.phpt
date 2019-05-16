--TEST--
Uninitialized result of PRE_INC/PRE_DEC in case of exception
--FILE--
<?php
declare(strict_types=1);
$o = new class {
	public string $a = "123";
};
$x = & $o->a;
try {
	$ret = ++$x + 5;
} catch (TypeError $e) {
}
?>
OK
--EXPECT--
OK
