--TEST--
Value classes preserve object equality and identity semantics
--FILE--
<?php
value class BookingId {
    public function __construct(public int|string $value) {}
}
value class OtherId {
    public function __construct(public int|string $value) {}
}
$a = new BookingId(123);
$b = new BookingId(123);
$alias = $a;
var_dump($a == $b, $a === $b, $a === $alias);
var_dump($a == new BookingId('123'));
var_dump($a == new BookingId(456), $a == new OtherId(123));
var_dump(spl_object_id($a) !== spl_object_id($b));
$weak = WeakReference::create($b);
$map = new WeakMap;
$map[$a] = 'a';
$map[$b] = 'b';
var_dump(count($map));
unset($b);
var_dump($weak->get(), count($map));
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
int(2)
NULL
int(1)
