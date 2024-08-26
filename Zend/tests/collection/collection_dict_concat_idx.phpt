--TEST--
Collection: Dictionary: Concat (int key)
--FILE--
<?php

class Book {
    public function __construct(public string $title) {}
}

collection(Dict) Books<int => Book> {}

$c1 = new Books();
$c2 = new Books();

$c1->add(41, new Book('Title 1'));
$c1->add(42, new Book('Title 2'));

$c2->add(43, new Book('Title 3'));
$c2->add(44, new Book('Title 4'));

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
    [41]=>
    object(Book)#%d (1) {
      ["title"]=>
      string(7) "Title 1"
    }
    [42]=>
    object(Book)#%d (1) {
      ["title"]=>
      string(7) "Title 2"
    }
    [43]=>
    object(Book)#%d (1) {
      ["title"]=>
      string(7) "Title 3"
    }
    [44]=>
    object(Book)#%d (1) {
      ["title"]=>
      string(7) "Title 4"
    }
  }
}
object(Books)#%d (1) {
  ["value"]=>
  array(2) {
    [41]=>
    object(Book)#%d (1) {
      ["title"]=>
      string(7) "Title 1"
    }
    [42]=>
    object(Book)#%d (1) {
      ["title"]=>
      string(7) "Title 2"
    }
  }
}
