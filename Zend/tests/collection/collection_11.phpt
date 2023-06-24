--TEST--
Modifying a sequence
--FILE--
<?php

class Book {
    public function __construct(public string $title) {}
}

collection(Seq) Books(Book) {}

$c = new Books();

$c->add(new Book('Title 1'));
$c->add(new Book('Title 2'));

// Should have 2 items.
var_dump($c);

$c->remove(new Book('Title 1'));

// Should have one item.
var_dump($c);

// Should be false
var_dump($c->has(new Book('Title 1')));
var_dump(isset($c[new Book('Title 1')]));

// Should be true
var_dump($c->has(new Book('Title 2')));
var_dump(isset($c[new Book('Title 2')]));

$c2 = $c->with(new Book('Title 3'));

// Still one item.
var_dump($c);

// But this has 2 items.
var_dump($c2);

$c3 = $c2->without(new Book('Title 2'));

// Still two items.
var_dump($c2);

// But only one item here.
var_dump($c3);

$c3->set(0, new Book('Title 4'));

// Only Title 4 now exists.
var_dump($c3);

unset($c3[0]);

// Empty
var_dump($c3);

?>
--EXPECTF--
