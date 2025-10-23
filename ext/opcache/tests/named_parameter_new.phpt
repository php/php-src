--TEST--
Named Parameters are optimized for known constructors
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
--SKIPIF--
<?php
    if(substr(PHP_OS, 0, 3) == 'WIN') die("skip on windows: Internal classes cannot be optimized");
?>
--EXTENSIONS--
opcache
--FILE--
<?php

final class MyClass
{
	private function __construct(
		private \Random\Engine $foo,
		private int $bar = 0,
	) {}

	public static function new(int $bar): self
	{
		$engine = new \Random\Engine\Xoshiro256StarStar(seed: 123);
		return new self(foo: $engine, bar: $bar);
	}
}

MyClass::new(bar: 1);

?>
--EXPECTF--
$_main:
     ; (lines=4, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 INIT_STATIC_METHOD_CALL 1 string("MyClass") string("new")
0001 SEND_VAL int(1) 1
0002 DO_UCALL
0003 RETURN int(1)

MyClass::__construct:
     ; (lines=7, args=2, vars=2, tmps=0)
     ; (after optimizer)
     ; %s
0000 CV0($foo) = RECV 1
0001 CV1($bar) = RECV_INIT 2 int(0)
0002 ASSIGN_OBJ THIS string("foo")
0003 OP_DATA CV0($foo)
0004 ASSIGN_OBJ THIS string("bar")
0005 OP_DATA CV1($bar)
0006 RETURN null

MyClass::new:
     ; (lines=10, args=1, vars=2, tmps=1)
     ; (after optimizer)
     ; %s
0000 CV0($bar) = RECV 1
0001 V2 = NEW 1 string("Random\\Engine\\Xoshiro256StarStar")
0002 SEND_VAL int(123) 1
0003 DO_FCALL
0004 CV1($engine) = QM_ASSIGN V2
0005 V2 = NEW 2 (self) (exception)
0006 SEND_VAR CV1($engine) 1
0007 SEND_VAR CV0($bar) 2
0008 DO_FCALL
0009 RETURN V2
LIVE RANGES:
     2: 0002 - 0004 (new)
     2: 0006 - 0009 (new)
