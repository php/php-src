--TEST--
Methods via variable name, bug #20120
--SKIP--
--FILE--
<?php
class bugtest {
        function bug() {
                echo "test\n";
        }
	function refbug() {
		echo "test2\n";
	}
}
$method='bug';
bugtest::$method();
$foo=&$method;
$method='refbug';
bugtest::$foo();

$t = new bugtest;
$t->$method();
?>
--EXPECT--
test
test2
test2
