--TEST--
Io\Terminal\Key: enum cases and identity
--FILE--
<?php

use Io\Terminal\Key;

var_dump(enum_exists(Key::class));

$cases = Key::cases();
echo "Total cases: " . count($cases) . "\n";

$expected = [
    'Up', 'Down', 'Right', 'Left', 'Enter', 'Backspace', 'Escape', 'Tab',
    'Home', 'End', 'Delete', 'PageUp', 'PageDown', 'Resize',
    'F1', 'F2', 'F3', 'F4', 'F5', 'F6', 'F7', 'F8', 'F9', 'F10', 'F11', 'F12'
];

$names = array_map(fn($c) => $c->name, $cases);
var_dump($names === $expected);

// Key identity and comparison
var_dump(Key::Up === Key::Up);
var_dump(Key::Up !== Key::Down);

?>
--EXPECT--
bool(true)
Total cases: 26
bool(true)
bool(true)
bool(true)
