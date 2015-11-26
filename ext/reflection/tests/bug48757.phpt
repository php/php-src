--TEST--
Bug #48757 (ReflectionFunction::invoke() parameter issues)
--FILE--
<?php
function test() {
	echo "Hello World\n";
}

function another_test($parameter) {
	var_dump($parameter);
}

$func = new ReflectionFunction('test');
$func->invoke();

$func = new ReflectionFunction('another_test');
$func->invoke('testing');
?>
--EXPECT--
Hello World
string(7) "testing"
