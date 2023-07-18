--TEST--
Collection: Dictionary: map (int key)
--FILE--
<?php

class Book {
    public function __construct(public string $title) {}
}

collection(Dict) Books<int => Book> {}

collection(Dict) Titles<int => string> {}

$c1 = new Books();

$c1->add(74, new Book('Title 1'));
$c1->add(75, new Book('Title 2'));

$c2 = $c1->map(fn(Book $b, int $k) => sprintf('%s (%d)', $b->title, $k), Titles::class);
var_dump($c2 instanceof Titles);
var_dump($c2);

?>
--EXPECTF--
bool(true)
object(Titles)#%d (%d) {
  ["value"]=>
  array(2) {
    [74]=>
    string(12) "Title 1 (74)"
    [75]=>
    string(12) "Title 2 (75)"
  }
}
