<?php

final struct Foo
{
    array $bar;
}

struct Customer
{
    use Foo;
    final int $id = 1;
    string $name;
    DateTimeImmutable $birthDate;
    $active;
}

var_dump(new Customer {
    id = 1,
    name = 'John Doe',
    birthDate = new DateTimeImmutable('@0'),
    active = true
});
