--TEST--
ZE2 Tests that a static defined unset will have the unset code called
--FILE--
<?php

class AccessorTest {
	public $b {
		get;
		set;
		isset;
		unset {
			echo get_called_class()."::\$b Unsetter Called.\n";
			self::$b = NULL;
		}
	}
}

echo "isset(AccessorTest::\$b) = ".(int)isset(AccessorTest::$b)."\n";
AccessorTest::$b = 10;
echo "AccessorTest::\$b = 10;\n";
echo "isset(AccessorTest::\$b) = ".(int)isset(AccessorTest::$b)."\n";
unset(AccessorTest::$b);
echo "unset(AccessorTest::\$b);\n";
echo "isset(AccessorTest::\$b) = ".(int)isset(AccessorTest::$b)."\n";
echo "Done";
?>
--EXPECTF--
isset(AccessorTest::$b) = 0
AccessorTest::$b = 10;
isset(AccessorTest::$b) = 1
AccessorTest::$b Unsetter Called.
unset(AccessorTest::$b);
isset(AccessorTest::$b) = 0
Done