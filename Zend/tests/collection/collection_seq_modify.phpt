--TEST--
Collection: Sequence: Modify
--FILE--
<?php

class Book {
    public function __construct(public string $title) {}
}

collection(Seq) Books<Book> {}

$c = new Books();

$c->add(new Book('Title 1'));
$c->add(new Book('Title 2'));

echo "\nShould have 2 items:\n";
var_dump($c);

$c->remove(0);

echo "\nShould have one item:\n";
var_dump($c);

echo "\nShould be false:\n";
var_dump($c->has(1));
var_dump(isset($c[1]));

echo "\nShould be true:\n";
var_dump($c->has(0));
var_dump(isset($c[0]));

$c2 = $c->with(new Book('Title 3'));

echo "\nStill one item:\n";
var_dump($c);

echo "\nBut this has 2 items:\n";
var_dump($c2);

$c3 = $c2->without(1);

echo "\nStill two items:\n";
var_dump($c2);

echo "\nBut only one item here:\n";
var_dump($c3);

$c3->set(0, new Book('Title 4'));

echo "\nOnly 'Title 4' now exists:\n";
var_dump($c3);

unset($c3[0]);

echo "\nEmpty:\n";
var_dump($c3);

echo "\nThrows OutOfBoundsException:\n";
try {
	$c3->set(42, new Book('Title 1'));
} catch (OutOfBoundsException $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
Should have 2 items:
object(Books)#1 (1) {
  ["value"]=>
  array(2) {
    [0]=>
    object(Book)#2 (1) {
      ["title"]=>
      string(7) "Title 1"
    }
    [1]=>
    object(Book)#3 (1) {
      ["title"]=>
      string(7) "Title 2"
    }
  }
}

Should have one item:
object(Books)#1 (1) {
  ["value"]=>
  array(1) {
    [0]=>
    object(Book)#3 (1) {
      ["title"]=>
      string(7) "Title 2"
    }
  }
}

Should be false:
bool(false)
bool(false)

Should be true:
bool(true)
bool(true)

Still one item:
object(Books)#1 (1) {
  ["value"]=>
  array(1) {
    [0]=>
    object(Book)#3 (1) {
      ["title"]=>
      string(7) "Title 2"
    }
  }
}

But this has 2 items:
object(Books)#4 (1) {
  ["value"]=>
  array(2) {
    [0]=>
    object(Book)#3 (1) {
      ["title"]=>
      string(7) "Title 2"
    }
    [1]=>
    object(Book)#2 (1) {
      ["title"]=>
      string(7) "Title 3"
    }
  }
}

Still two items:
object(Books)#4 (1) {
  ["value"]=>
  array(2) {
    [0]=>
    object(Book)#3 (1) {
      ["title"]=>
      string(7) "Title 2"
    }
    [1]=>
    object(Book)#2 (1) {
      ["title"]=>
      string(7) "Title 3"
    }
  }
}

But only one item here:
object(Books)#5 (1) {
  ["value"]=>
  array(1) {
    [0]=>
    object(Book)#3 (1) {
      ["title"]=>
      string(7) "Title 2"
    }
  }
}

Only 'Title 4' now exists:
object(Books)#5 (1) {
  ["value"]=>
  array(1) {
    [0]=>
    object(Book)#6 (1) {
      ["title"]=>
      string(7) "Title 4"
    }
  }
}

Empty:
object(Books)#5 (1) {
  ["value"]=>
  array(0) {
  }
}

Throws OutOfBoundsException:
OutOfBoundsException: Index '42' does not exist in the Books sequence
