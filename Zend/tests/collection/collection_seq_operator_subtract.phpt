--TEST--
Collection: Sequence: Concat
--FILE--
<?php

class Book {
    public function __construct(public string $title) {}
}

collection(Seq) Books<Book> {}

$c1 = new Books();
$c2 = new Books();

$c1->add(new Book('Title 1'));
$c1->add(new Book('Title 2'));

$c2->add(new Book('Title 3'));
$c2->add(new Book('Title 4'));

$c3 = $c1 + $c2;
$c4 = $c3 - $c2;

// Two items again
var_dump($c4);

?>
--EXPECTF--
object(Books)#%d (1) {
  ["value"]=>
  array(2) {
    [0]=>
    object(Book)#%d (1) {
      ["title"]=>
      string(7) "Title 1"
    }
    [1]=>
    object(Book)#%d (1) {
      ["title"]=>
      string(7) "Title 2"
    }
  }
}
