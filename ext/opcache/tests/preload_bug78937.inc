<?php
spl_autoload_register(function($class) {
    echo "Loading $class\n";
    eval("class $class {}");
});

function foo() {
    return new class extends AnonDep {};
}
function bar($bool) {
    if ($bool) {
        class Foo extends FooDep {
            public $id = 1;
        }
        return new Foo;
    } else {
        class Foo extends FooDep2 {
            public $id = 2;
        }
        return new Foo;
    }
}
