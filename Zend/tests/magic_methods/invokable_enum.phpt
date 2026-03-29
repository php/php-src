--TEST--
Enums with __invoke() auto-implement Invokable
--FILE--
<?php

/* Enum with __invoke auto-implements Invokable */
enum Color {
    case Red;
    case Blue;
    public function __invoke(): string { return $this->name; }
}

var_dump(Color::Red instanceof Invokable);
var_dump((Color::Red)());

/* Enum explicitly implementing Invokable */
enum Direction implements Invokable {
    case Up;
    case Down;
    public function __invoke(): int { return $this === self::Up ? 1 : -1; }
}

var_dump(Direction::Up instanceof Invokable);
var_dump((Direction::Up)());

?>
--EXPECT--
bool(true)
string(3) "Red"
bool(true)
int(1)
