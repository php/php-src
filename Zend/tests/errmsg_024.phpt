--TEST--
errmsg: cannot change initial value of property
--FILE--
<?php

class test1 {
	static protected $var = 1;
}

class test extends test1 {
	static $var = 10;
}

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Cannot change initial value of property static protected test1::$var in class test in %s on line %d
