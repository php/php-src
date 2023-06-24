--TEST--
Mapping a dictionary
--FILE--
<?php

class Book {
    public function __construct(public string $title) {}
}

collection(Dict) Books(string => Book) {}

collection(Dict) Titles(string => string) {}

$c1 = new Books();

$c1->add('one', new Book('Title 1'));
$c1->add('two', new Book('Title 2'));

$c2 = $c1->map(fn(Book $b, string $k) => sprintf('%s: %s', $k, $book->title), Titles::class);

var_dump($c2 instanceof Titles);

// Should be a map of string to string, like one => one: Title 1.
var_dump($c2);

?>
--EXPECTF--
