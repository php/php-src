--TEST--
Bug #24871 (methods misidentified as constructors)
--FILE--
<?php
class a { }

class b {
	function bb() {
		var_dump(method_exists($this, "bb"));
	}
}

$a = new a();
aggregate($a, "b");
$a->bb();
?>
--EXPECT--
bool(true)
