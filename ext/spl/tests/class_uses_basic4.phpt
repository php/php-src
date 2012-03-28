--TEST--
SPL: Test class_uses() function : basic
--FILE--
<?php
/* Prototype  : array class_uses(mixed what [, bool autoload ], [, string trait_name])
 * Description: Return all traits used by a class or check if a trait is used
 * Source code: ext/spl/php_spl.c
 * Alias to functions:
 */

echo "*** Testing class_uses() : basic ***\n";


trait foo { }
class fooUser { use foo; }

trait bar { }
class barUser { use bar; }

class foobarUser { use foo, bar; }

/** There is no semantics for traits in the inheritance chain.
    Traits are flattend into a class, and that semantics is nothing
    like a type, or interface, and thus, not propergated. */
class fooViaBarUser extends barUser { use foo; }

class fooExtended extends fooUser {}

var_dump(class_uses(new foobarUser, false, 'foo'));
var_dump(class_uses(new foobarUser, false, 'bar'));
var_dump(class_uses(new foobarUser, false, 'baz'));
var_dump(class_uses('foobarUser', false, 'foo'));
var_dump(class_uses('foobarUser', false, 'bar'));
var_dump(class_uses('foobarUser', false, 'baz'));

var_dump(class_uses(new fooViaBarUser, false, 'foo'));
var_dump(class_uses(new fooViaBarUser, false, 'bar'));
var_dump(class_uses(new fooViaBarUser, false, 'baz'));
var_dump(class_uses('fooViaBarUser', false, 'foo'));
var_dump(class_uses('fooViaBarUser', false, 'bar'));
var_dump(class_uses('fooViaBarUser', false, 'baz'));

var_dump(class_uses(new fooExtended, false, 'foo'));
var_dump(class_uses(new fooExtended, false, 'bar'));
var_dump(class_uses(new fooExtended, false, 'baz'));
var_dump(class_uses('fooExtended', false, 'foo'));
var_dump(class_uses('fooExtended', false, 'bar'));
var_dump(class_uses('fooExtended', false, 'baz'));

?>
===DONE===
--EXPECT--
*** Testing class_uses() : basic ***
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
===DONE===
