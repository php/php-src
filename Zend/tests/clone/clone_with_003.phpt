--TEST--
Clone with supports property hooks
--FILE--
<?php

class Clazz {
	public string $hooked = 'default' {
		set {
			$this->hooked = strtoupper($value);
		}
	}
}

$c = new Clazz();

var_dump(clone($c, [ 'hooked' => 'updated' ]));

?>
--EXPECTF--
object(Clazz)#%d (1) {
  ["hooked"]=>
  string(7) "UPDATED"
}
