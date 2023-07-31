--TEST--
Collection: Dictionary: Concat (string key)
--FILE--
<?php

class Book {
    public function __construct(public string $title) {}
}

collection(Dict) Books<string => Book> {}

$c1 = new Books();
$c2 = new Books();

$c1->add('one', new Book('Title 1'));
$c1->add('two', new Book('Title 2'));

$c2->add('three', new Book('Title 3'));
$c2->add('four', new Book('Title 4'));

$c3 = $c1->concat($c2);

// Four items.
var_dump($c3);

// Still two items.
var_dump($c1);

?>
--EXPECTF--
object(Books)#%d (1) {
  ["value"]=>
  array(4) {
    ["one"]=>
    object(Book)#%d (1) {
      ["title"]=>
      string(7) "Title 1"
    }
    ["two"]=>
    object(Book)#%d (1) {
      ["title"]=>
      string(7) "Title 2"
    }
    ["three"]=>
    object(Book)#%d (1) {
      ["title"]=>
      string(7) "Title 3"
    }
    ["four"]=>
    object(Book)#%d (1) {
      ["title"]=>
      string(7) "Title 4"
    }
  }
}
object(Books)#%d (1) {
  ["value"]=>
  array(2) {
    ["one"]=>
    object(Book)#%d (1) {
      ["title"]=>
      string(7) "Title 1"
    }
    ["two"]=>
    object(Book)#%d (1) {
      ["title"]=>
      string(7) "Title 2"
    }
  }
}
