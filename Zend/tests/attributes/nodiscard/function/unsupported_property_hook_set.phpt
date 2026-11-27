--TEST--
#[\NoDiscard]: Not allowed on 'set' property hook.
--FILE--
<?php

class Clazz {
	public string $test {
		#[\NoDiscard]
		set(string $value) {
			$this->test = $value;
		}
	}
}

$cls = new Foo();
$cls->test = 'foo';

?>
--EXPECTF--
Fatal error: #[\NoDiscard] is not supported for property hooks in %s on line %d
