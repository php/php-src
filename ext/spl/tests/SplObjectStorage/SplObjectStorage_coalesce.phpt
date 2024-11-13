--TEST--
SplObjectStorage magic operators
--FILE--
<?php
$i = 'dynamic';
$v = $i . '_';
$s = new SplObjectStorage();
$o1 = new Stdclass;
$o2 = new Stdclass;
var_dump($s[$o1] ?? 'default');
var_dump($s[$o1] ??= $i);
var_dump($s[$o1] ??= null);
var_dump($s[$o1] ??= false);
var_dump($s[$o1] ?? $i);
var_dump(isset($s[$o1]));
var_dump(empty($s[$o1]));
echo "o2\n";
var_dump(isset($s[$o2]));
var_dump(empty($s[$o2]));
$s[$o2] = null;
var_dump($s[$o2] ?? new stdClass());
echo "check isset/empty/contains for null. offsetExists returns true as long as the entry is there.\n";
var_dump(isset($s[$o2]));
var_dump(empty($s[$o2]));
var_dump($s->contains($o2));
echo "check isset/empty/contains for false.\n";
$s[$o2] = false;
var_dump(isset($s[$o2]));
var_dump(empty($s[$o2]));
var_dump($s->contains($o2));
try {
    $s['invalid'] = 123;
} catch (Error $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}
try {
    var_dump(isset($s['invalid']));
} catch (Error $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}
$a = &$s[$o1];

var_dump($s);

?>
--EXPECTF--
string(7) "default"
string(7) "dynamic"
string(7) "dynamic"
string(7) "dynamic"
string(7) "dynamic"
bool(true)
bool(false)
o2
bool(false)
bool(true)
object(stdClass)#4 (0) {
}
check isset/empty/contains for null. offsetExists returns true as long as the entry is there.
bool(true)
bool(true)
bool(true)
check isset/empty/contains for false.
bool(true)
bool(true)
bool(true)
TypeError: SplObjectStorage::offsetSet(): Argument #1 ($object) must be of type object, string given
TypeError: SplObjectStorage::offsetExists(): Argument #1 ($object) must be of type object, string given

Notice: Indirect modification of overloaded element of SplObjectStorage has no effect in %s on line 38
object(SplObjectStorage)#1 (1) {
  ["storage":"SplObjectStorage":private]=>
  array(2) {
    [0]=>
    array(2) {
      ["obj"]=>
      object(stdClass)#2 (0) {
      }
      ["inf"]=>
      string(7) "dynamic"
    }
    [1]=>
    array(2) {
      ["obj"]=>
      object(stdClass)#3 (0) {
      }
      ["inf"]=>
      bool(false)
    }
  }
}