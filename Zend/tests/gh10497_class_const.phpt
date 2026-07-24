--TEST--
GH-10497: Allow direct modification of object properties on class constants
--FILE--
<?php

// A class constant cannot use `new` directly, but it may reference a global
// constant that holds an object (see GH-10497 discussion).
const BACKING = new stdClass;

class C {
    const O = BACKING;
}

// Access via class name.
C::O->prop = 123;
var_dump(C::O->prop);

// Access via instance (static-property-like syntax on an object).
$c = new C;
$c::O->prop = 'overwritten';
var_dump(C::O->prop);

// Compound assignment, increment/decrement and concatenation.
C::O->counter = 0;
C::O->counter++;
C::O->counter++;
C::O->counter--;
var_dump(C::O->counter);

C::O->str = 'hello';
C::O->str .= ' world';
var_dump(C::O->str);

// Nested property chains.
C::O->inner = new stdClass;
C::O->inner->value = 999;
var_dump(C::O->inner->value);

// isset()/unset().
C::O->temp = 'remove me';
var_dump(isset(C::O->temp));
unset(C::O->temp);
var_dump(isset(C::O->temp));

// Passing by reference.
function incr(&$v) { $v++; }
C::O->reftest = 10;
incr(C::O->reftest);
var_dump(C::O->reftest);

// self:: from within a method. The same backing object is reachable through
// the class constant and the global constant, so the mutation is visible.
class D {
    const O = BACKING;

    public static function set(): void {
        self::O->fromSelf = 'yes';
    }
}

D::set();
var_dump(D::O->fromSelf);
var_dump(BACKING->fromSelf);

?>
--EXPECT--
int(123)
string(11) "overwritten"
int(1)
string(11) "hello world"
int(999)
bool(true)
bool(false)
int(11)
string(3) "yes"
string(3) "yes"
