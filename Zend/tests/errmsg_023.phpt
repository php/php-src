--TEST--
errmsg: access level must be the same or weaker
--FILE--
<?php

class test1 {
	protected $var;
}

class test extends test1 { 
	private $var;
}

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Access level to test::$var must be protected (as in class test1) or weaker in %s on line %d
