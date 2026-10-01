--TEST--
GH-10497: Destructuring and by-reference foreach targeting constant object properties
--FILE--
<?php

const BACKING = new stdClass;

class C {
    const O = BACKING;
}

// By-reference foreach must write through to the referenced object, as it does
// for a plain variable holding the same object.
BACKING->arr = [1, 2, 3];
foreach (BACKING->arr as &$v) {
    $v *= 2;
}
unset($v);
var_dump(BACKING->arr);

C::O->arr = [1, 2, 3];
foreach (C::O->arr as &$v) {
    $v *= 2;
}
unset($v);
var_dump(C::O->arr);

// Nested chains and a dimension after the property fetch.
BACKING->inner = new stdClass;
BACKING->inner->arr = [1, 2, 3];
foreach (BACKING->inner->arr as &$v) {
    $v *= 2;
}
unset($v);
var_dump(BACKING->inner->arr);

BACKING->matrix = [[1, 2]];
foreach (BACKING->matrix[0] as &$v) {
    $v *= 2;
}
unset($v);
var_dump(BACKING->matrix[0]);

// Destructuring assignment.
[BACKING->p, BACKING->q] = [1, 2];
var_dump(BACKING->p, BACKING->q);

['k' => BACKING->keyed] = ['k' => 9];
var_dump(BACKING->keyed);

BACKING->list = [];
[BACKING->list[0]] = [7];
var_dump(BACKING->list);

[C::O->viaClassConst] = ['yes'];
var_dump(BACKING->viaClassConst);

?>
--EXPECT--
array(3) {
  [0]=>
  int(2)
  [1]=>
  int(4)
  [2]=>
  int(6)
}
array(3) {
  [0]=>
  int(2)
  [1]=>
  int(4)
  [2]=>
  int(6)
}
array(3) {
  [0]=>
  int(2)
  [1]=>
  int(4)
  [2]=>
  int(6)
}
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(4)
}
int(1)
int(2)
int(9)
array(1) {
  [0]=>
  int(7)
}
string(3) "yes"
