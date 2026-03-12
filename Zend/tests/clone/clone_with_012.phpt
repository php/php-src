--TEST--
Clone with property hook updating readonly property
--FILE--
<?php

class Clazz {
	public string $foo {
		set {
			$this->foo = $value;
			$this->bar = 'bar updated in hook';
		}
	}

	public public(set) readonly string $bar;
}

$f = new Clazz();

var_dump(clone($f, ['foo' => 'foo updated in clone-with']));

try {
	var_dump(clone($f, ['foo' => 'foo updated in clone-with', 'bar' => 'bar updated in clone-with']));
} catch (Throwable $e) {
	echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
object(Clazz)#%d (2) {
  ["foo"]=>
  string(25) "foo updated in clone-with"
  ["bar"]=>
  string(19) "bar updated in hook"
}
Error: Cannot modify readonly property Clazz::$bar
