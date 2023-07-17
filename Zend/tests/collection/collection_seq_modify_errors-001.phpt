--TEST--
Collection: Sequence: Modify with unsupported key types
--FILE--
<?php

class Book {
    public function __construct(public string $title) {}
}

collection(Seq) Books<Book> {}

$c = new Books();

try {
	$c->remove('zero');
} catch (TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	$c->has(new Book("Ne pas un cle"));
} catch (TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	$c->get(false);
} catch (TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	$c->without(M_PI);
} catch (TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	$c->set(null, new Book("A Random Title"));
} catch (TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
TypeError: Key type string of element does not match Books sequence key type int
TypeError: Key type Book of element does not match Books sequence key type int
TypeError: Key type bool of element does not match Books sequence key type int
TypeError: Key type float of element does not match Books sequence key type int
TypeError: Key type null of element does not match Books sequence key type int
