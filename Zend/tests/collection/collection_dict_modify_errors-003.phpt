--TEST--
Collection: Dictionary: Modify with unsupported key types
--FILE--
<?php

class Book {
    public function __construct(public string $title) {}
}

collection(Dict) Books<string => Book> {}

$c = new Books();

try {
	$c->add(M_PI, new Book('Title 1'));
} catch (\TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	$c->remove(M_PI);
} catch (\TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	$c->has(new Book("This is not a key"));
} catch (\TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	isset($c[M_PI]);
} catch (\TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	$c->with(false, new Book("Pie is tasty"));
} catch (\TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	$c->without(NULL);
} catch (\TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	$c->set(M_PI, new Book("Pai e cho blasta"));
} catch (\TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
TypeError: Key type float of element does not match Books dictionary key type string
TypeError: Key type float of element does not match Books dictionary key type string
TypeError: Key type Book of element does not match Books dictionary key type string
TypeError: Key type float of element does not match Books dictionary key type string
TypeError: Key type bool of element does not match Books dictionary key type string
TypeError: Key type null of element does not match Books dictionary key type string
TypeError: Key type float of element does not match Books dictionary key type string
