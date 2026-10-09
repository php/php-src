--TEST--
Test Csv\collection_to_buffer() and Csv\collection_to_file(): a row freed by a field's __toString() is not used after free
--DESCRIPTION--
Converting a field to string can run userland code that frees the row being formatted: resuming
the generator that produced it, or overwriting the element of an ArrayIterator. The row must
stay alive until it has been formatted.
--EXTENSIONS--
csv
--FILE--
<?php
class ResumesGenerator {
    public function __toString(): string {
        $GLOBALS['gen']->next();
        return str_repeat('x', 8);
    }
}
function rows(): Generator {
    yield [new ResumesGenerator, str_repeat('a', 64), str_repeat('b', 64)];
    yield ['c', 'd', 'e'];
}

$gen = rows();
var_dump(Csv\collection_to_buffer($gen));

$file = __DIR__ . '/row_freed_by_tostring.csv';
$gen = rows();
Csv\collection_to_file($file, $gen);
var_dump(file_get_contents($file));

class OverwritesRow {
    public function __toString(): string {
        $GLOBALS['it'][0] = ['z', 'z', 'z'];
        return 'y';
    }
}
$it = new ArrayIterator([[new OverwritesRow, str_repeat('a', 64), str_repeat('b', 64)]]);
var_dump(Csv\collection_to_buffer($it));
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/row_freed_by_tostring.csv');
?>
--EXPECT--
string(140) "xxxxxxxx,aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa,bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
"
string(140) "xxxxxxxx,aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa,bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
"
string(133) "y,aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa,bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
"
