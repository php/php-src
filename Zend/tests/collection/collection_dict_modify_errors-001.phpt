--TEST--
Collection: Dictionary: Modify with wrong key type (string)
--FILE--
<?php

class Book {
    public function __construct(public string $title) {}
}

collection(Dict) Books<string => Book> {}

$c = new Books();

$c->add('one', new Book('Title 1'));
$c->add('two', new Book('Title 2'));

try {
	$c->remove(1);
} catch (\TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	$c->has(1);
} catch (\TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	isset($c[1]);
} catch (\TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	$c->with(3, new Book('Title 3'));
} catch (\TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	$c->without(2);
} catch (\TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	$c->set(3, new Book('Title 4'));
} catch (\TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
TypeError: Key type int of element does not match Books dictionary key type string
TypeError: Key type int of element does not match Books dictionary key type string
TypeError: Key type int of element does not match Books dictionary key type string
TypeError: Key type int of element does not match Books dictionary key type string
TypeError: Key type int of element does not match Books dictionary key type string
TypeError: Key type int of element does not match Books dictionary key type string
