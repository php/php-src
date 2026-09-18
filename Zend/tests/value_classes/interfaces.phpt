--TEST--
Value classes implement interfaces, including Stringable and property contracts
--FILE--
<?php
interface Identifier {
    public string $value { get; }
    public function toString(): string;
}
value class BookingId implements Identifier, Stringable {
    public function __construct(public string $value) {}
    public function toString(): string { return $this->value; }
    public function __toString(): string { return $this->value; }
}
$id = new BookingId('booking-123');
var_dump($id instanceof Identifier, $id instanceof Stringable);
var_dump($id->toString(), (string) $id);
?>
--EXPECT--
bool(true)
bool(true)
string(11) "booking-123"
string(11) "booking-123"
