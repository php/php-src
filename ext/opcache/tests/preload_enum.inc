<?php

enum MyEnum {
    case Foo;
    case Bar;
}
var_dump(MyEnum::Bar);

class Test {
    public $x = MyEnum::Bar;
}
new Test;

if (false) {
    enum MyEnum2 {
        case Foo;
    }
}
