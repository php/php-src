--TEST--
array_intersect() preserves element selection when conversion retains a hash table with holes
--FILE--
<?php
class CapturingStringableWithHole {
    public static array $argument;

    public function __toString(): string {
        self::$argument = debug_backtrace()[1]['args'][0];
        return 'drop';
    }
}

function temporary_argument_with_hole(): array {
    $array = [
        'hole' => null,
        'drop' => new CapturingStringableWithHole(),
        'keep' => 'keep',
    ];
    unset($array['hole']);
    return $array;
}

$result = array_intersect(temporary_argument_with_hole(), ['keep']);
var_dump(array_keys($result), array_keys(CapturingStringableWithHole::$argument));
?>
--EXPECT--
array(1) {
  [0]=>
  string(4) "keep"
}
array(2) {
  [0]=>
  string(4) "drop"
  [1]=>
  string(4) "keep"
}
