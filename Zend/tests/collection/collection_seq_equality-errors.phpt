--TEST--
Collection: Sequence: Equals Errors
--FILE--
<?php
declare(strict_types=1);

class Book {
    public function __construct(public string $title) {}
}

collection(Seq) Books<Book> {}
collection(Seq) BooksOther<Book> {}

$c1 = new Books();
$c1->add(new Book('Title 1'));
$c1->add(new Book('Title 2'));

try {
	$c1->equals(new Book('Title 3'));
} catch (TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	$c1->equals(null);
} catch (TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	var_dump($c1 == new Book('Title 3'));
} catch (TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	var_dump(new Book('Title 3') == $c1);
} catch (TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	var_dump($c1 == M_PI);
} catch (TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	var_dump(false == $c1);
} catch (TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
TypeError: Books::equals(): Argument #1 ($other) must be of type SeqCollection, Book given
TypeError: Books::equals(): Argument #1 ($other) must be of type SeqCollection, null given
bool(false)
bool(false)
bool(false)
bool(false)
