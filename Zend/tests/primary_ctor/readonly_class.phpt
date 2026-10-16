--TEST--
Primary constructors: readonly class promotes readonly properties
--FILE--
<?php
readonly class Money(public int $amount, public string $currency = 'USD') {
    public function format(): string { return "{$this->amount} {$this->currency}"; }
}

$m = new Money(500);
echo $m->format(), "\n";

try {
    $m->amount = 1;
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
500 USD
Cannot modify readonly property Money::$amount
