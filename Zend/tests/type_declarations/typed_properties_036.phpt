--TEST--
Test unitialized typed properties normal foreach must not be yielded
--FILE--
<?php
$foo = new class {
	public int $bar = 10, $qux;
};

foreach ($foo as $key => $bar) {
	var_dump($key, $bar);
}
?>
--EXPECT--
string(3) "bar"
int(10)
