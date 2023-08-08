--TEST--
Collection: Dictionary: Comparison Handler
--FILE--
<?php

class Book {
    public function __construct(public string $title) {}
}

collection(Dict) Books<string => Book> {}
collection(Dict) BooksOther<string => Book> {}
collection(Dict) YearBooks<int => Book> {}

$c1 = new Books();
$c1->add('one', new Book('Title 1'));
$c1->add('two', new Book('Title 2'));

$c2 = new Books();
$c2->add('one', new Book('Title 1'));
$c2->add('two', new Book('Title 2'));

$c3 = new Books();
$c3->add('one', new Book('Title 1'));
$c3->add('two', new Book('Title 2'));
$c3->add('tri', new Book('Title 3'));

$c4 = new Books();
$c4->add('one', new Book('Title 1'));
$c4->add('XXX', new Book('Title X'));
$c4->add('two', new Book('Title 2'));

$c5 = new Books();
$c5->add('one', new Book('Title 1'));
$c5->add('two', new Book('Title X'));
$c5->add('tri', new Book('Title 2'));

$c6 = new Books();
$c6->add('one', new Book('Title 1'));
$c6->add('XXX', new Book('Title 2'));
$c6->add('tri', new Book('Title 3'));

$c7 = new BooksOther();
$c7->add('one', new Book('Title 1'));
$c7->add('two', new Book('Title 2'));

$c8 = new YearBooks();
$c8->add(1970, new Book('Title 1'));
$c8->add(1971, new Book('Title 2'));

$c9 = new Books();
$c9->add('two', new Book('Title 2'));
$c9->add('one', new Book('Title 1'));

// True
var_dump($c1 == $c1);
var_dump($c1 == $c2);
var_dump($c1 == $c7);
var_dump($c1 == $c9);

$c2['tri'] = new Book('Title 3');

var_dump($c2 == $c3);

// False
var_dump($c1 == $c2);
var_dump($c1 == $c8);
var_dump($c3 == $c4);
var_dump($c3 == $c5);
var_dump($c3 == $c6);

unset($c4['XXX']);

// True
var_dump($c1 == $c4);

unset($c6['XXX']);
$c6['two'] = new Book('Title 2');

// True
var_dump($c3 == $c6);
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
