--TEST--
Value classes support readonly-compatible trait properties and methods
--FILE--
<?php
trait Identifier {
    public readonly string $value;
    public function __construct(string $value) { $this->value = $value; }
    public function text(): string { return $this->value; }
}
value class BookingId {
    use Identifier { text as toString; }
}
$id = new BookingId('booking-123');
var_dump($id->value, $id->toString());
try {
    $id->value = 'changed';
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
string(11) "booking-123"
string(11) "booking-123"
Cannot modify readonly property BookingId::$value
