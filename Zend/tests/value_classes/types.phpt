--TEST--
Value class properties support nullable, union, enum and value class types
--FILE--
<?php
declare(strict_types=1);
enum Currency { case EUR; }
value class Money {
    public function __construct(public int $amount, public Currency $currency) {}
}
value class Payment {
    public ?Money $money;
    public function __construct(?Money $money, public int|string|null $reference = null) {
        $this->money = $money;
    }
}
$payment = new Payment(new Money(100, Currency::EUR), 'booking-123');
var_dump($payment->money->amount, $payment->money->currency, $payment->reference);
var_dump((new Payment(null))->money, (new Payment(null))->reference);
var_dump((new Payment(null, 123))->reference);
try {
    new Payment(null, []);
} catch (TypeError $e) {
    echo "TypeError\n";
}
try {
    $payment->money = null;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
int(100)
enum(Currency::EUR)
string(11) "booking-123"
NULL
NULL
int(123)
TypeError
Cannot modify readonly property Payment::$money
