--TEST--
Primary constructors: a later promoted property hook can initialize derived state
--FILE--
<?php
class Range(
    public int $start,
    public int $end {
        set {
            if ($this->start > $value) {
                throw new ValueError('start must precede end');
            }
            $this->end = $value;
            $this->length = $value - $this->start;
        }
    }
) {
    public readonly int $length;
}

$range = new Range(2, 5);
var_dump($range);

try {
    $range->length = 99;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
object(Range)#1 (3) {
  ["start"]=>
  int(2)
  ["end"]=>
  int(5)
  ["length"]=>
  int(3)
}
Cannot modify readonly property Range::$length
