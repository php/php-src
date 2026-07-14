--TEST--
Returning a refcounted parent CV from a scope-fn body must not null the parent CV
--FILE--
<?php
function test() {
    $x = new stdClass;
    $x->v = 'hello';
    $f = fn() {
        return $x; /* parent CV; ZEND_RETURN's cv-to-result move would null $x without the OBSERVED guard */
    };
    $r = $f();
    var_dump($x->v);     // must still be 'hello' — parent CV not nulled
    var_dump($r === $x); // result is same object
}
test();
--EXPECT--
string(5) "hello"
bool(true)
