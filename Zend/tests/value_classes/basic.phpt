--TEST--
Value classes support construction, promoted properties and methods
--FILE--
<?php
value class BookingId
{
    public function __construct(public string $value) {}

    public function toString(): string
    {
        return $this->value;
    }
}

$id = new BookingId('booking-123');
var_dump($id->value);
var_dump($id->toString());
$reflection = new ReflectionClass(BookingId::class);
var_dump($reflection->isFinal(), $reflection->isReadOnly());
var_dump($reflection->getProperty('value')->isPromoted());
var_dump($reflection->getProperty('value')->isReadOnly());
?>
--EXPECT--
string(11) "booking-123"
string(11) "booking-123"
bool(true)
bool(true)
bool(true)
bool(true)
