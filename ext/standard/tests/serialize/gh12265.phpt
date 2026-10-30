--TEST--
GH-12265 (Cloning an object breaks serialization recursion) - __serialize variation
--FILE--
<?php

class A {
    public function __construct(public B $x) {
    }
}

class B {
    public A $a;

    public function __serialize()
    {
        return ['a' => new A($this)];
    }
}

class C {
    public B $b;

    public function __construct() {
        $this->b = new B;
    }
}

$b = new B();
$sb = serialize($b);
$stb = serialize(new B);

printf("serialized original:    %s\n", $sb);
printf("serialized temp    :    %s\n", $stb);

$c = new C;
$sc = serialize($c);
$stc = serialize(new C);

printf("serialized original:    %s\n", $sc);
printf("serialized temp    :    %s\n", $stc);

?>
--EXPECT--
serialized original:    O:1:"B":1:{s:1:"a";O:1:"A":1:{s:1:"x";r:1;}}
serialized temp    :    O:1:"B":1:{s:1:"a";O:1:"A":1:{s:1:"x";r:1;}}
serialized original:    O:1:"C":1:{s:1:"b";O:1:"B":1:{s:1:"a";O:1:"A":1:{s:1:"x";r:2;}}}
serialized temp    :    O:1:"C":1:{s:1:"b";O:1:"B":1:{s:1:"a";O:1:"A":1:{s:1:"x";r:2;}}}
