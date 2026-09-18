--TEST--
ReflectionClass and ReflectionObject expose value classes
--FILE--
<?php
class Regular {}
final readonly class FinalReadonly {}
readonly class ReadonlyClass {}
interface Contract {}
trait Methods {}
enum Choice { case One; }
value class BookingId {
    public function __construct(public string $value) {}
}
foreach ([Regular::class, FinalReadonly::class, ReadonlyClass::class,
        Contract::class, Methods::class, Choice::class, stdClass::class, BookingId::class] as $class) {
    echo $class, ': ';
    var_dump((new ReflectionClass($class))->isValue());
}
$reflection = new ReflectionClass(BookingId::class);
var_dump($reflection->isFinal(), $reflection->isReadOnly());
var_dump($reflection->getModifiers() === (ReflectionClass::IS_FINAL | ReflectionClass::IS_READONLY));
var_dump(str_contains((string) $reflection, 'final readonly value class BookingId'));
var_dump((new ReflectionObject(new BookingId('booking-123')))->isValue());
class_alias(BookingId::class, 'AliasId');
var_dump((new ReflectionClass('AliasId'))->isValue());
$method = new ReflectionMethod(ReflectionClass::class, 'isValue');
var_dump((string) $method->getReturnType(), $method->getNumberOfParameters());
?>
--EXPECT--
Regular: bool(false)
FinalReadonly: bool(false)
ReadonlyClass: bool(false)
Contract: bool(false)
Methods: bool(false)
Choice: bool(false)
stdClass: bool(false)
BookingId: bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
string(4) "bool"
int(0)
