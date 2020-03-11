<?php
spl_autoload_register(function($class) {
    echo "Loading $class\n";
    eval("class $class {}");
});

function foo() {
    return new class extends AnonDep {};
}
function bar() {
	class Foo extends FooDep {
	}
    return new Foo;
}
