--TEST--
ZE2 A static abstrcat methods
--FILE--
<?php

interface showable
{
	static function show();
}

class pass implements showable
{
	static function show() {
		echo "Call to function show()\n";
	}
}

pass::show();

eval('
class fail
{
	abstract static function func();
}
');

fail::show();

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
Call to function show()

Fatal error: Static function fail::func() cannot be abstract in %s on line %d
