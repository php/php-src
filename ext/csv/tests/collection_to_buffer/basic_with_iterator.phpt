--TEST--
Test Csv\collection_to_buffer() with an Iterator as a collection
--SKIPIF--
<?php
if (!extension_loaded('csv')) {
	echo 'skip';
}
?>
--FILE--
<?php

class Collection implements Iterator {
    private const ROWS = [
        [
            'One',
            'Two',
            'Three',
        ],
        [
            'Four',
            'Five',
            'Six',
        ],
        [
            'Seven',
            'Eight',
            'Nine',
        ],
    ];

    protected int $i = 0;

    public function rewind(): void {
        $this->i = 0;
    }

    public function valid(): bool {
        return $this->i < 3;
    }

    public function current(): array {
        return self::ROWS[$this->i];
    }

    public function key(): int {
        return $this->i;
    }

    public function next(): void {
        $this->i++;
    }

    public function __destruct() {}
}

$collection = new Collection();

$output = "One,Two,Three\r\nFour,Five,Six\r\nSeven,Eight,Nine\r\n";

var_dump($output === Csv\collection_to_buffer($collection));
var_dump(Csv\collection_to_buffer($collection));
var_dump(Csv\collection_to_buffer($collection, ','));
var_dump(Csv\collection_to_buffer($collection, ',', '"'));
var_dump(Csv\collection_to_buffer($collection, ',', '"', "\r\n"));

?>
--EXPECT--
bool(true)
string(48) "One,Two,Three
Four,Five,Six
Seven,Eight,Nine
"
string(48) "One,Two,Three
Four,Five,Six
Seven,Eight,Nine
"
string(48) "One,Two,Three
Four,Five,Six
Seven,Eight,Nine
"
string(48) "One,Two,Three
Four,Five,Six
Seven,Eight,Nine
"
