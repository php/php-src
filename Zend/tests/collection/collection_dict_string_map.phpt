--TEST--
Collection: Dictionary: map (string key)
--FILE--
<?php

class Book {
    public function __construct(public string $title) {}
}

collection(Dict) Books<string => Book> {}

collection(Dict) Titles<string => string> {}

$c1 = new Books();

$c1->add('one', new Book('Title 1'));
$c1->add('two', new Book('Title 2'));

$c2 = $c1->map(fn(Book $b, string $k) => sprintf('%s: %s', $k, $b->title), Titles::class);
var_dump($c2 instanceof Titles);
var_dump($c2);

?>
--EXPECTF--
bool(true)
object(Titles)#%d (%d) {
  ["value"]=>
  array(2) {
    ["one"]=>
    string(12) "one: Title 1"
    ["two"]=>
    string(12) "two: Title 2"
  }
}
