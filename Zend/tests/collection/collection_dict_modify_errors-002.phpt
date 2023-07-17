--TEST--
Collection: Dictionary: Modify with wrong key type (int)
--FILE--
<?php

class Book {
    public function __construct(public string $title) {}
}

collection(Dict) Books<int => Book> {}

$c = new Books();

$c->add(1, new Book('Title 1'));
$c->add(2, new Book('Title 2'));

try {
	$c->remove('one');
} catch (\TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	$c->has('one');
} catch (\TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	isset($c['one']);
} catch (\TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	$c->with('three', new Book('Title 3'));
} catch (\TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	$c->without('two');
} catch (\TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	$c->set('three', new Book('Title 4'));
} catch (\TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
TypeError: Key type string of element does not match Books dictionary key type int
TypeError: Key type string of element does not match Books dictionary key type int
TypeError: Key type string of element does not match Books dictionary key type int
TypeError: Key type string of element does not match Books dictionary key type int
TypeError: Key type string of element does not match Books dictionary key type int
TypeError: Key type string of element does not match Books dictionary key type int
