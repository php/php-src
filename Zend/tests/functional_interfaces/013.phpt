--TEST--
functional interfaces: call method without ce and proxy
--FILE--
<?php
class Foo {
	private $bar = [];

	public function fill($limit = 100) {
		for ($i = 0; $i < $limit; $i++) {
			$this->bar[] = mt_rand($i, $limit);
		}
	}

	public function getEvenCounter() : Countable {
		return function () implements Countable {
			$counter = 0;
			foreach ($this->bar as $value) {
				if ($value % 2 === 0)
					$counter++;
			}
			return $counter;
		};
	}

	public function getOddCounter() : Countable {
		return function () implements Countable {
			$counter = 0;
			foreach ($this->bar as $value) {
				if ($value % 2 !== 0) {
					$counter++;
				}
			}
			return $counter;
		};
	}
}

$foo = new Foo();

$even = $foo->getEvenCounter();
$odd = $foo->getOddCounter();

$it = 0;

while (++$it<10) {
	$foo->fill(50);
	var_dump(
		count($even),
		count($odd));
}
?>
--EXPECTF--
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
