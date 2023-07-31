--TEST--
Collection: Dictionary: Concat Errors
--FILE--
<?php

class Book {
    public function __construct(public string $title) {}
}
class DutchBook extends Book {}

class Article {
    public function __construct(public string $subject) {}
}

collection(Dict) Books<string => Book> {}
collection(Dict) YBooks<int => Book> {}
collection(Dict) DutchBooks<string => DutchBook> {}
collection(Dict) Articles<string => Article> {}

$c1 = new Books();
$c1->add("one", new Book('Title 1'));
$c1->add("two", new Book('Title 2'));

$y1 = new YBooks();
$y1->add(1970, new Book('Title 1'));
$y1->add(1971, new Book('Title 2'));

$d1 = new DutchBooks();
$d1->add("één", new DutchBook('Titel 1'));
$d1->add("twee", new DutchBook('Titel 2'));

$a2 = new Articles();
$a2->add("three", new Article('Subject 1'));
$a2->add("four", new Article('Subject 2'));

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

try {
	$why = $c1->concat($y1);
	var_dump( $why );
} catch (TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	$why = $y1->concat($c1);
	var_dump( $why );
} catch (TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: Books::concat(): Argument #1 ($other) must be of type DictCollection, Book given
TypeError: Value type Books does not match Article collection item type Book
Inheritence: OK
TypeError: Value type DutchBooks does not match Book collection item type DutchBook
TypeError: Key type int of element does not match Books dictionary key type string
TypeError: Key type string of element does not match YBooks dictionary key type int
