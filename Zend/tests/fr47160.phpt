--TEST--
Calling method from array
--FILE--
<?php

class Hello {
	public function world($x) {
		echo "Hello, $x\n";return $this;
	}
}

class Hello2 {
	static public function world($x) {
		echo "Hello, $x\n";
	}
}

class Magic {
	public function __call($f, $a) {
		printf("%s called (%s)!\n", __METHOD__, $f);
	}
}

class Magic2 {
	public static function __callStatic($f, $a) {
		printf("%s called (%s)!\n", __METHOD__, $f);
	}
}

class Magic3 {
	public static function __callStatic($f, $a) {
		printf("%s called (%s)!\n", __METHOD__, $f);
	}
	public function __call($f, $a) {
		printf("%s called (%s)!\n", __METHOD__, $f);
	}
}

$f = array('Hello','world');
try {
	var_dump($f('you'));
} catch (Throwable $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}
try {
	var_dump(call_user_func($f, 'you'));
} catch (Throwable $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}

printf("-----\n");

$h= new Hello;
$f = array($h,'world');
var_dump($f('again'));
var_dump(call_user_func($f, 'again'));

printf("-----\n");

function bar() {
	return array(new Hello,'world');
}
$f = bar();
var_dump($f('there'));
var_dump(call_user_func($f, 'there'));

printf("-----\n");

$x = function ($c,$v) { return array($c, $v); };

$c = new Hello;
$m = 'world';
$f = $x($c, $m);
var_dump($f('devs'));
var_dump(call_user_func($f, 'devs'));

printf("-----\n");

$f = array(new Magic, 'foo');
$f();
call_user_func($f);

printf("-----\n");

$f = array('Magic2', 'foo');
$f();
call_user_func($f);


printf("-----\n");

$f = array('Magic3', 'foo');
$f();
call_user_func($f);

printf("-----\n");

$f = array(new Magic3, 'foo');
$f();
call_user_func($f);

printf("-----\n");

$f = array(new Hello2, 'world');
var_dump($f('you'));
var_dump(call_user_func($f, 'you'));

?>
--EXPECTF--
Deprecated: Non-static method Hello::world() should not be called statically in %s on line %d
Hello, you
Exception: Using $this when not in object context

Deprecated: %son-static method Hello::world() should not be called statically in %s on line %d
Hello, you
Exception: Using $this when not in object context
-----
Hello, again
object(Hello)#%d (0) {
}
Hello, again
object(Hello)#%d (0) {
}
-----
Hello, there
object(Hello)#%d (0) {
}
Hello, there
object(Hello)#%d (0) {
}
-----
Hello, devs
object(Hello)#%d (0) {
}
Hello, devs
object(Hello)#%d (0) {
}
-----
Magic::__call called (foo)!
Magic::__call called (foo)!
-----
Magic2::__callStatic called (foo)!
Magic2::__callStatic called (foo)!
-----
Magic3::__callStatic called (foo)!
Magic3::__callStatic called (foo)!
-----
Magic3::__call called (foo)!
Magic3::__call called (foo)!
-----
Hello, you
NULL
Hello, you
NULL
