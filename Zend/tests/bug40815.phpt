--TEST--
Bug #40815 (using strings like "class::func" and static methods in set_exception_handler() might result in crash).
--FILE--
<?php

class ehandle{
	static public function exh ($ex) {
		echo 'foo';
	}
}

set_exception_handler("ehandle::exh");

throw new Exception ("Whiii");
echo "Done\n";
?>
--EXPECTF--	
foo
