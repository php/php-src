--TEST--
Concatenating a sequence
--FILE--
<?php

class Book {
    public function __construct(public string $title) {}
}

collection(Seq) Books(Book) {}

$c1 = new Books();
$c2 = new Books();

$c1->add(new Book('Title 1'));
$c1->add(new Book('Title 2'));

$c2->add(new Book('Title 3'));
$c2->add(new Book('Title 4'));

$c3 = $c->concat($c2);

// Four items.
var_dump($c3);

// Still two items.
var_dump($c1);

?>
--EXPECTF--
