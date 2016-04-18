--TEST--
Function is polymorphic if its input is equal to or less than what is required
and its output is equal or more specific than what is required.
If it has nested input callable constraints, with each level argument variance should be inverted.
--FILE--
<?php

class A {}
class B extends A {}

function elephant(callable(callable(A)) $chipmunk) {
    $chipmunk("giraffe");
};

function chipmunk(callable(B) $giraffe) {
    $giraffe(new B);
}

function giraffe(A $a) {
    var_dump($a);
}

elephant("chipmunk");

// -------------------------------------------

function beetle(callable: A $fly) {
    var_dump($fly());
};

function fly(): B {
    return new B;
}

beetle("fly");

// -------------------------------------------

function ostrich(callable: callable: B $pidgeotto) {
    var_dump($pidgeotto()());
};

function pidgeotto(): callable: B {
    return "pidgeot";
}

function pidgeot(): B {
    return new B;
}

ostrich("pidgeotto");

// -------------------------------------------

function hummingbird(callable(callable(callable(callable(A)))) $falcon) {
    $falcon("eagle");
}

function falcon(callable(callable(callable(B))) $eagle) {
    $eagle("seagull");
}

function eagle(callable(callable(A)) $seagull) {
    $seagull("condor");
}

function seagull(callable(B) $condor) {
    $condor(new B);
}

function condor(A $a) {
    var_dump($a);
}

hummingbird("falcon");

// -------------------------------------------

function chimp(callable(callable(A)) $dog) {
    $dog("rat");
}

function dog(callable(B, $a) $rat) {
    $rat(new B, 1);
}

function rat(A $a) {
    var_dump($a);
}

?>
--EXPECTF--
object(B)#%d (0) {
}
object(B)#%d (0) {
}
object(B)#%d (0) {
}
object(B)#%d (0) {
}
