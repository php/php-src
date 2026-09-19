--TEST--
Primary constructors: later promoted property hooks can validate against earlier promoted properties
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
        }
    }
) {}

$ok = new Range(1, 3);
var_dump($ok);

try {
    new Range(3, 1);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
object(Range)#1 (2) {
  ["start"]=>
  int(1)
  ["end"]=>
  int(3)
}
start must precede end
