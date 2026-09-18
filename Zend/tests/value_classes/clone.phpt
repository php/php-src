--TEST--
Value classes inherit readonly cloning and clone-with behavior
--FILE--
<?php
value class BookingId {
    public function __construct(public string $value) {}

    public function withValue(string $value): self {
        return clone($this, ['value' => $value]);
    }
}
$id = new BookingId('first');
$copy = clone $id;
var_dump($copy == $id, $copy === $id, $copy->value);
$changed = $id->withValue('second');
var_dump($id->value, $changed->value);
foreach ([$copy, $changed] as $value) {
    try {
        $value->value = 'third';
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}
var_dump((new ReflectionClass(BookingId::class))->isCloneable());
?>
--EXPECT--
bool(true)
bool(false)
string(5) "first"
string(5) "first"
string(6) "second"
Cannot modify readonly property BookingId::$value
Cannot modify readonly property BookingId::$value
bool(true)
