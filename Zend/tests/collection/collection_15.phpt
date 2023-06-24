--TEST--
Mapping a sequence
--FILE--
<?php

class Book {
    public function __construct(public string $title) {}
}

collection(Seq) Books(Book) {}

collection(Seq) Titles(string) {}

$c1 = new Books();

$c1->add(new Book('Title 1'));
$c1->add(new Book('Title 2'));

$c2 = $c1->map(fn(Book $b): string => $b->title), Titles::class);

// True
var_dump($c2 instanceof Titles);

// List of 2 strings
var_dump($c2);

?>
--EXPECTF--
