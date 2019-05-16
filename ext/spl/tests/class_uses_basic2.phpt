--TEST--
SPL: Test class_uses() function : basic
--FILE--
<?php
/* Prototype  : array class_uses(mixed what [, bool autoload ])
 * Description: Return all traits used by a class
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

s_var_dump(class_uses(new foobarUser));
s_var_dump(class_uses('foobarUser'));
s_var_dump(class_uses(new fooViaBarUser));
s_var_dump(class_uses('fooViaBarUser'));
s_var_dump(class_uses(new fooExtended));
s_var_dump(class_uses('fooExtended'));


function s_var_dump($arr) {
   krsort($arr);
   var_dump($arr);
}
?>
===DONE===
--EXPECT--
*** Testing class_uses() : basic ***
array(2) {
  ["foo"]=>
  string(3) "foo"
  ["bar"]=>
  string(3) "bar"
}
array(2) {
  ["foo"]=>
  string(3) "foo"
  ["bar"]=>
  string(3) "bar"
}
array(1) {
  ["foo"]=>
  string(3) "foo"
}
array(1) {
  ["foo"]=>
  string(3) "foo"
}
array(0) {
}
array(0) {
}
===DONE===
