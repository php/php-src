--TEST--
Modifying a dictionary
--FILE--
<?php

class Book {
    public function __construct(public string $title) {}
}

collection(Dict) Books(string => Book) {}

$c = new Books();

$c->add('one', new Book('Title 1'));
$c->add('two', new Book('Title 2'));

// Should have 2 items.
var_dump($c);

$c->remove('one');

// Should have one item.
var_dump($c);

// Should be false
var_dump($c->has('one'));
var_dump(isset($c['one']));

// Should be true
var_dump($c->has('two'));
var_dump(isset($c['two']));

$c2 = $c->with('three', new Book('Title 3'));

// Still one item.
var_dump($c);

// But this has 2 items.
var_dump($c2);

$c3 = $c2->without('two');

// Still two items.
var_dump($c2);

// But only one item here.
var_dump($c3);

$c3->set('three', new Book('Title 4'));

// Only Title 4 now exists.
var_dump($c3);

unset($c3['three']);

// Empty
var_dump($c3);

?>
--EXPECTF--
