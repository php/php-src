--TEST--
#[\NoDiscard]: Not allowed on 'get' property hook.
--FILE--
<?php

class Clazz {
	public string $test {
		#[\NoDiscard]
		get {
			return 'asd';
		}
	}
}

$cls = new Clazz();
$cls->test;

?>
--EXPECTF--
Fatal error: #[\NoDiscard] is not supported for property hooks in %s on line %d
