--TEST--
Dictionary equality
--FILE--
<?php

class Book {
    public function __construct(public string $title) {}
}

collection(Dict) Books(string => Book) {}

$c1 = new Books();
$c2 = new Books();

$c1->add('one', new Book('Title 1'));
$c1->add('two', new Book('Title 2'));

$c2->add('one', new Book('Title 1'));
$c2->add('two', new Book('Title 2'));

// True
var_dump($c1->equals($c2));
var_dump($c1 == $c2);

$c2['three'] = new Book('Title 3');

// False
var_dump($c1->equals($c2));
var_dump($c1 == $c2);

?>
--EXPECTF--
