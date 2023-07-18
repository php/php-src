--TEST--
Collection: Dictionary: map errors
--FILE--
<?php

class Book {
    public function __construct(
		public string $title,
		public int $copyright,
	) {}
}

class NormalTitle {
}

collection(Dict) Titles<string => string> {}

collection(Dict) Books<string => Book> {}

$c1 = new Books();

$c1->add('seventy-eight', new Book('Title 1', 1978));
$c1->add('twenty-three', new Book('Title 2', 2023));

try {
	$c2 = $c1->map(fn(Book $b, string $k): string => "{$k}: {$b->title}", Unknown::class);
} catch (TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}
echo "\n";

try {
	$c2 = $c1->map(fn(Book $b, string $k): string => "{$k}: {$b->title}", NormalTitle::class);
} catch (TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}
echo "\n";

try {
	$c2 = $c1->map(fn(Book $b, string $k): int => "{$k}: {$b->title}", Titles::class);
} catch (TypeError $e) {
	echo get_class($e->getPrevious()), ': ', $e->getPrevious()->getMessage(), "\n";
	echo get_class($e), ': ', $e->getMessage(), "\n";
}
echo "\n";

try {
	$c2 = $c1->map(fn(Book $b): string => "{$k}: {$b->title}", Titles::class);
} catch (TypeError $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}
echo "\n";

try {
	$c3 = $c1->map(fn(Book $b, int $k) => sprintf('%d: %s', $k, $b->title), Titles::class);
} catch (TypeError $e) {
	echo get_class($e->getPrevious()), ': ', $e->getPrevious()->getMessage(), "\n";
	echo get_class($e), ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
TypeError: Type 'Unknown' can not be fetched

TypeError: Type 'NormalTitle' must implement DictCollection interface

TypeError: {closure}(): Return value must be of type int, string returned
TypeError: Value type Titles does not match null collection item type string


Warning: Undefined variable $k in %scollection_dict_map_error.php on line 45

Warning: Undefined variable $k in %scollection_dict_map_error.php on line 45

TypeError: {closure}(): Argument #2 ($k) must be of type int, string given
TypeError: Value type Titles does not match null collection item type string
