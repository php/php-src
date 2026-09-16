--TEST--
Clone with error handling
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
				throw new \Exception('Length insufficient');
			}

			$this->minLength = $value;
		}
	}
}

$c = new Clazz();

try {
	var_dump(clone($c, [ 'hooked' => 'updated', 'maxLength' => 'abcdef', 'minLength' => 'abc' ]));
} catch (Throwable $e) {
	echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

echo PHP_EOL;

try {
	var_dump(clone($c, [ 'hooked' => 'updated', 'minLength' => 'abc', 'maxLength' => 'abcdef' ]));
} catch (Throwable $e) {
	echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
$hooked::set
$maxLength::set
Exception: Length exceeded

$hooked::set
$minLength::set
Exception: Length insufficient
