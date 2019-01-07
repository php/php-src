--TEST--
foreach() must return properly typed references
--FILE--
<?php
class Foo {
	public int $bar = 0;
	public float $baz = 0.5;
}

foreach ($foo = new Foo as $k => &$val) {
	var_dump($val);

	$val = 20;
	var_dump($foo->$k);

	try {
		$val = [];
		var_dump($foo->$k);
	} catch (Throwable $e) {
		echo $e->getMessage(), "\n";
	}
}
?>
--EXPECT--
int(0)
int(20)
Cannot assign array to reference held by property Foo::$bar of type int
float(0.5)
float(20)
Cannot assign array to reference held by property Foo::$baz of type float
