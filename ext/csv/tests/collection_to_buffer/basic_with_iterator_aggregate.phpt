--TEST--
Test Csv\collection_to_buffer() with a IteratorAggregate as a collection
--SKIPIF--
<?php
if (!extension_loaded('csv')) {
	echo 'skip';
}
?>
--FILE--
<?php

class Collection implements IteratorAggregate {
    public $property1 = ['One', 'Two', 'Three'];
    public $property2 = ['Four', 'Five', 'Six'];
    public $property3 = ['Seven', 'Eight', 'Nine'];

    public function __construct() {
        /* Suppress deprecation notice about dynamic property creation */
        @$this->property4 = ['Ten', 'Eleven', 'Twelve'];
    }

    public function getIterator(): Traversable {
        $blah = $this;
        return (function () use ($blah) {
            yield $blah->property1;
            yield $blah->property2;
            yield $blah->property3;
            yield $blah->property4;
        })();
    }
}

$collection = new Collection();

$output = "One,Two,Three\r\nFour,Five,Six\r\nSeven,Eight,Nine\r\nTen,Eleven,Twelve\r\n";

var_dump($output === Csv\collection_to_buffer($collection));
var_dump(Csv\collection_to_buffer($collection));
var_dump(Csv\collection_to_buffer($collection, ','));
var_dump(Csv\collection_to_buffer($collection, ',', '"'));
var_dump(Csv\collection_to_buffer($collection, ',', '"', "\r\n"));

?>
--EXPECT--
bool(true)
string(67) "One,Two,Three
Four,Five,Six
Seven,Eight,Nine
Ten,Eleven,Twelve
"
string(67) "One,Two,Three
Four,Five,Six
Seven,Eight,Nine
Ten,Eleven,Twelve
"
string(67) "One,Two,Three
Four,Five,Six
Seven,Eight,Nine
Ten,Eleven,Twelve
"
string(67) "One,Two,Three
Four,Five,Six
Seven,Eight,Nine
Ten,Eleven,Twelve
"
