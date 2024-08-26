--TEST--
Collection: Dictionary/Sequence: Equals
--FILE--
<?php

class Book {
    public function __construct(public string $title) {}
}

collection(Seq) Books<Book> {}
collection(Dict) YearBooks<int => Book> {}

$s1 = new Books();
$s1->add(new Book('Title 1'));
$s1->add(new Book('Title 2'));

$d1 = new YearBooks();
$d1->add(0, new Book('Title 1'));
$d1->add(1, new Book('Title 2'));

// Exception
try {
	var_dump($s1->equals($d1));
} catch (TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	var_dump($d1->equals($s1));
} catch (TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
TypeError: Books::equals(): Argument #1 ($other) must be of type SeqCollection, YearBooks given
TypeError: YearBooks::equals(): Argument #1 ($other) must be of type DictCollection, Books given
