<?php
declare(strict_types=1);

enum Currency { case EUR; }

value class Money
{
    public function __construct(public int $amount, public Currency $currency) {}
}

function doublePrice(): int
{
    $money = new Money(100, Currency::EUR);
    return $money->amount * 2;
}

value class FixedMoney
{
    public int $amount = 100;
}

function doubleFixedPrice(): int
{
    $money = new FixedMoney();
    return $money->amount * 2;
}

var_dump(doublePrice(), doubleFixedPrice());
