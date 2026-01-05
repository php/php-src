--TEST--
Binding pattern
--FILE--
<?php

class Box {
    public function __construct(
        public $value,
    ) {}
}

class NotBox {
    public function __construct(
        public $value,
    ) {}
}

class Many {
    public function __construct(
        public $a = 1,
        public $b = 2,
        public $c = 3,
        public $d = 4,
        public $e = 5,
        public $f = 6,
        public $g = 7,
        public $h = 8,
        public $i = 9,
        public $j = 10,
    ) {}
}

var_dump(10 is $a);
var_dump($a);

var_dump('Hello world' is $a);
var_dump($a);

var_dump(new Box(42) is Box(value: $a));
var_dump($a);

var_dump(new NotBox(43) is Box(value: $a));
var_dump($a);

var_dump(43 is $a & int);
var_dump($a);

var_dump([] is $a & string);
var_dump($a);

var_dump(new Many() is Many(:$a, :$b, :$c, :$d));
var_dump($a, $b, $c, $d, isset($e));

var_dump(new Many() is Many(:$a, :$b, :$c, :$d, :$e, :$f, :$g, :$h, :$i, :$j));
var_dump($a, $b, $c, $d, $e, $f, $g, $h, $i, $j);

?>
--EXPECT--
bool(true)
int(10)
bool(true)
string(11) "Hello world"
bool(true)
int(42)
bool(false)
int(42)
bool(true)
int(43)
bool(false)
int(43)
bool(true)
int(1)
int(2)
int(3)
int(4)
bool(false)
bool(true)
int(1)
int(2)
int(3)
int(4)
int(5)
int(6)
int(7)
int(8)
int(9)
int(10)
