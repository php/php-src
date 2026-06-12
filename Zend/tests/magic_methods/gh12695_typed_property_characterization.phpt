--TEST--
GH-12695: Characterize current behavior with typed properties
--FILE--
<?php

/* For typed declared properties that are uninitialized (never assigned),
 * the engine deliberately skips __isset (see zend_object_handlers.c
 * "Skip __isset() for uninitialized typed properties"). This test pins
 * down that skip and the related behavior on `unset()`'d typed props.
 */

class T {
    public int $never;
    public int $explicitlyUnset = 99;
    public function __get($n) { echo "  __get($n)\n"; return 42; }
    public function __isset($n) { echo "  __isset($n)\n"; return true; }
}

echo "1) `??` on never-initialized typed property: __isset is skipped, fallback used:\n";
$t = new T;
var_dump($t->never ?? 'fallback');

echo "\n2) isset() on never-initialized typed property: __isset is skipped, returns false:\n";
$t = new T;
var_dump(isset($t->never));

echo "\n3) `??` after explicit unset() on a typed property: __isset and __get are called:\n";
$t = new T;
unset($t->explicitlyUnset);
var_dump($t->explicitlyUnset ?? 'fallback');

echo "\n4) isset() after explicit unset() on a typed property: only __isset is called:\n";
$t = new T;
unset($t->explicitlyUnset);
var_dump(isset($t->explicitlyUnset));

?>
--EXPECT--
1) `??` on never-initialized typed property: __isset is skipped, fallback used:
string(8) "fallback"

2) isset() on never-initialized typed property: __isset is skipped, returns false:
bool(false)

3) `??` after explicit unset() on a typed property: __isset and __get are called:
  __isset(explicitlyUnset)
  __get(explicitlyUnset)
int(42)

4) isset() after explicit unset() on a typed property: only __isset is called:
  __isset(explicitlyUnset)
bool(true)
