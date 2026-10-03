--TEST--
Primary constructors: promoted property hooks cannot read later promoted properties before initialization
--FILE--
<?php
class Range(
    public int $start {
        set {
            var_dump(isset($this->end));
            var_dump($this->end);
            $this->start = $value;
        }
    },
    public int $end,
) {}

try {
    new Range(1, 3);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(false)
Typed property Range::$end must not be accessed before initialization
