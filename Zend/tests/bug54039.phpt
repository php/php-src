--TEST--
Bug #54039 (use() of static variables in lambda functions can break staticness)
--FILE--
<?php
function test_1() {
	static $v = 0;
	++$v;
	echo "Outer function increments \$v to $v\n";
	$f = function() use($v) {
		echo "Inner function reckons \$v is $v\n";
	};
	return $f;
}

$f = test_1(); $f();
$f = test_1(); $f();

function test_2() {
	static $v = 0;
	$f = function() use($v) {
		echo "Inner function reckons \$v is $v\n";
	};
	++$v;
	echo "Outer function increments \$v to $v\n";
	return $f;
}

$f = test_2(); $f();
$f = test_2(); $f();

function test_3() {
	static $v = "";
	$v .= 'b';
	echo "Outer function catenates 'b' onto \$v to give $v\n";
	$f = function() use($v) {
		echo "Inner function reckons \$v is $v\n";
	};
	$v .= 'a';
	echo "Outer function catenates 'a' onto \$v to give $v\n";
	return $f;
}
$f = test_3(); $f();
$f = test_3(); $f();
--EXPECT--
Outer function increments $v to 1
Inner function reckons $v is 1
Outer function increments $v to 2
Inner function reckons $v is 2
Outer function increments $v to 1
Inner function reckons $v is 0
Outer function increments $v to 2
Inner function reckons $v is 1
Outer function catenates 'b' onto $v to give b
Outer function catenates 'a' onto $v to give ba
Inner function reckons $v is b
Outer function catenates 'b' onto $v to give bab
Outer function catenates 'a' onto $v to give baba
Inner function reckons $v is bab
