--TEST--
Collection: Sequence: map errors
--FILE--
<?php

class Book {
    public function __construct(
		public string $title,
		public int $copyright,
	) {}
}

class NormalTitle {
}

collection(Seq) Titles<string> {}

collection(Seq) Books<Book> {}

$c1 = new Books();

$c1->add(new Book('Title 1', 1978));
$c1->add(new Book('Title 2', 2023));

try {
	$c2 = $c1->map(fn(Book $b): string => $b->title, Unknown::class);
} catch (TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	$c2 = $c1->map(fn(Book $b): string => $b->title, NormalTitle::class);
} catch (TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	$c2 = $c1->map(fn(Book $b): int => $b->copyright, Titles::class);
} catch (TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
TypeError: Type 'Unknown' can not be fetched
TypeError: Type 'NormalTitle' must implement SeqCollection interface
TypeError: Value type Titles does not match int collection item type string
