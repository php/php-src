--TEST--
=== on structs
--FILE--
<?php

struct Box {
    public function __construct(
        public $value,
    ) {}
}

$a = new Box(1);
$b = new Box(2);
$c = $a;
$d = $a;
$d->value++;
$e = new Box('1');

$values = [$a, $b, $c, $d, $e];
foreach ($values as $l) {
    foreach ($values as $r) {
        echo var_export($l->value, true)
            . ' === '
            . var_export($r->value, true)
            . ' => '
            . ($l === $r ? 'true' : 'false')
            . "\n";
    }
}

#[\AllowDynamicProperties]
struct Point {}

$a = new Point();
$a->x = 1;
$a->y = 1;
$b = new Point();
$b->y = 1;
$b->x = 1;
var_dump($a === $b);
unset($b->y);
$b->y = 1;
var_dump($a === $b);

?>
--EXPECT--
1 === 1 => true
1 === 2 => false
1 === 1 => true
1 === 2 => false
1 === '1' => false
2 === 1 => false
2 === 2 => true
2 === 1 => false
2 === 2 => true
2 === '1' => false
1 === 1 => true
1 === 2 => false
1 === 1 => true
1 === 2 => false
1 === '1' => false
2 === 1 => false
2 === 2 => true
2 === 1 => false
2 === 2 => true
2 === '1' => false
'1' === 1 => false
'1' === 2 => false
'1' === 1 => false
'1' === 2 => false
'1' === '1' => true
bool(false)
bool(true)
