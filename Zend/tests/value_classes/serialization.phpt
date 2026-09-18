--TEST--
Value classes retain ordinary readonly object serialization
--FILE--
<?php
value class BookingId {
    public function __construct(public string $value) {}
}
$id = new BookingId('booking-123');
$serialized = serialize($id);
echo $serialized, "\n";
$copy = unserialize($serialized);
var_dump($copy->value, $copy == $id, $copy === $id);
try {
    $copy->value = 'changed';
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    unserialize('O:9:"BookingId":1:{s:5:"value";i:123;}');
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
value class Custom {
    public function __construct(public int $number) {}
    public function __serialize(): array { return ['n' => $this->number]; }
    public function __unserialize(array $data): void { $this->number = $data['n']; }
}
$custom = unserialize(serialize(new Custom(42)));
var_dump($custom->number);
try {
    $custom->__unserialize(['n' => 43]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
O:9:"BookingId":1:{s:5:"value";s:11:"booking-123";}
string(11) "booking-123"
bool(true)
bool(false)
Cannot modify readonly property BookingId::$value
Cannot assign int to property BookingId::$value of type string
int(42)
Cannot modify readonly property Custom::$number
