--TEST--
unserialize() with wrong-case enum name fails with wrong case
--FILE--
<?php
enum Suit: string {
    case Hearts = 'H';
    case Spades = 'S';
}

// Correct case works
$s = unserialize('E:11:"Suit:Hearts";');
echo $s->name . "\n";

// Wrong case is not found
$s2 = unserialize('E:11:"SUIT:Hearts";');
var_dump($s2);
?>
--EXPECTF--
Hearts

Warning: unserialize(): Class 'SUIT' not found in %s on line %d

Warning: unserialize(): Error at offset 0 of 19 bytes in %s on line %d
bool(false)
