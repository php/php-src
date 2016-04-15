--TEST--
functional interfaces: call method handling
--FILE--
<?php
class Foo {
	private $bar = [];

	public function __construct(array $bar) {
		$this->bar = $bar;
	}

	public function getShufflingIterator() : Traversable {

		return function() implements IteratorAggregate : Traversable {
			shuffle($this->bar);

			return new ArrayIterator($this->bar);
		};
	}
}

$foo = new Foo([
	"hello",
	"world"
]);

$shuffler = 
	$foo->getShufflingIterator();

foreach ($shuffler as $key => $value) {
	var_dump($value);
}

foreach ($shuffler as $key => $value) {
	var_dump($value);
}
?>
--EXPECTF--
string(5) "%s"
string(5) "%s"
string(5) "%s"
string(5) "%s"
