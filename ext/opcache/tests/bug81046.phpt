--TEST--
Bug #81046: Literal compaction merges non-equal related literals
--FILE--
<?php

class Test {
	static function methoD() {
        echo "Method called\n";
	}
}

const methoD = 1;
var_dump(methoD);
test::methoD();

?>
--EXPECT--
int(1)
Method called
