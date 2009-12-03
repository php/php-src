--TEST--
Bug #49472 (Constants defined in Interfaces can be overridden)
--FILE--
<?php

interface ia {
    const c = 'Sea';
    const y = 2;
}

class Foo implements ia {
}

class FooBar extends Foo implements ia {
	const x = 1;
	const c = 'Ocean';
	
	public function show() {
		return ia::c;
	}
}

new FooBar;

?>
--EXPECTF--
Fatal error: Cannot inherit previously-inherited or override constant c from interface ia in %s on line %d
