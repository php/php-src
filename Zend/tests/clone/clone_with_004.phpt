--TEST--
Clone with evaluation order
--FILE--
<?php

class Clazz {
	public string $hooked = 'default' {
		set {
			echo __FUNCTION__, PHP_EOL;

			$this->hooked = strtoupper($value);
		}
	}

	public string $maxLength {
		set {
			echo __FUNCTION__, PHP_EOL;

			if (strlen($value) > 5) {
				throw new \Exception('Length exceeded');
			}

			$this->maxLength = $value;
		}
	}

	public string $minLength {
		set {
			echo __FUNCTION__, PHP_EOL;

			if (strlen($value) < 5) {
				throw new \Exception('Length unsufficient');
			}

			$this->minLength = $value;
		}
	}
}

$c = new Clazz();

var_dump(clone($c, [ 'hooked' => 'updated' ]));
echo PHP_EOL;
var_dump(clone($c, [ 'hooked' => 'updated', 'maxLength' => 'abc', 'minLength' => 'abcdef' ]));
echo PHP_EOL;
var_dump(clone($c, [ 'minLength' => 'abcdef', 'hooked' => 'updated', 'maxLength' => 'abc' ]));

?>
--EXPECTF--
$hooked::set
object(Clazz)#%d (1) {
  ["hooked"]=>
  string(7) "UPDATED"
  ["maxLength"]=>
  uninitialized(string)
  ["minLength"]=>
  uninitialized(string)
}

$hooked::set
$maxLength::set
$minLength::set
object(Clazz)#%d (3) {
  ["hooked"]=>
  string(7) "UPDATED"
  ["maxLength"]=>
  string(3) "abc"
  ["minLength"]=>
  string(6) "abcdef"
}

$minLength::set
$hooked::set
$maxLength::set
object(Clazz)#%d (3) {
  ["hooked"]=>
  string(7) "UPDATED"
  ["maxLength"]=>
  string(3) "abc"
  ["minLength"]=>
  string(6) "abcdef"
}
