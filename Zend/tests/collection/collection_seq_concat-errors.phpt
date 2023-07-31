--TEST--
Collection: Sequence: Concat Errors
--FILE--
<?php

class Book {
    public function __construct(public string $title) {}
}
class DutchBook extends Book {}

class Article {
    public function __construct(public string $subject) {}
}

collection(Seq) Books<Book> {}
collection(Seq) DutchBooks<DutchBook> {}
collection(Seq) Articles<Article> {}

$c1 = new Books();
$c1->add(new Book('Title 1'));
$c1->add(new Book('Title 2'));

$d1 = new DutchBooks();
$d1->add(new DutchBook('Titel 1'));
$d1->add(new DutchBook('Titel 2'));

$a2 = new Articles();
$a2->add(new Article('Subject 1'));
$a2->add(new Article('Subject 2'));

try {
	$c1->concat(new Book('Title E'));
} catch (TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	$c1->concat($a2);
} catch (TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	$cr = $c1->concat($d1);
	echo "Inheritence: OK\n";
} catch (TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	$cr = $d1->concat($c1);
} catch (TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: Books::concat(): Argument #1 ($other) must be of type SeqCollection, Book given
TypeError: Value type Books does not match Article collection item type Book
Inheritence: OK
TypeError: Value type DutchBooks does not match Book collection item type DutchBook
