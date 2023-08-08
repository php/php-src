--TEST--
Collection: Sequence: Equals
--FILE--
<?php

class Book {
    public function __construct(public string $title) {}
}

collection(Seq) Books<Book> {}
collection(Seq) BooksOther<Book> {}


$c1 = new Books();
$c1->add(new Book('Title 1'));
$c1->add(new Book('Title 2'));

$c2 = new Books();
$c2->add(new Book('Title 1'));
$c2->add(new Book('Title 2'));

$c3 = new Books();
$c3->add(new Book('Title 1'));
$c3->add(new Book('Title 2'));
$c3->add(new Book('Title 3'));

$c4 = new Books();
$c4->add(new Book('Title 1'));
$c4->add(new Book('Title X'));
$c4->add(new Book('Title 2'));

$c5 = new BooksOther();
$c5->add(new Book('Title 1'));
$c5->add(new Book('Title 2'));

// True
var_dump($c1->equals($c1));
var_dump($c1->equals($c2));
var_dump($c1->equals($c5));

$c2[] = new Book('Title 3');

// False
var_dump($c1->equals($c2));
var_dump($c3->equals($c4));

unset($c3[2]);
unset($c4[1]);

// True
var_dump($c1->equals($c3));

// True (indexes have been automatically renumbered)
var_dump($c1->equals($c4));
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(true)
