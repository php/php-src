--TEST--
Ensure type hints are enforced for functions invoked as callbacks.
--FILE--
<?php

  echo "---> Type hints with callback function:\n";
  class A  {  }
  function f1(A $a)  {
      echo "in f1;\n";
  }
  function f2(A $a = null)  {
      echo "in f2;\n";
  }
  try {
    call_user_func('f1', 1);
  } catch (Error $ex) {
    echo "{$ex->getCode()}: {$ex->getMessage()} - {$ex->getFile()}({$ex->getLine()})\n\n";
  }
  try {
    call_user_func('f1', new A);
  } catch (Error $ex) {
    echo "{$ex->getCode()}: {$ex->getMessage()} - {$ex->getFile()}({$ex->getLine()})\n\n";
  }
  try {
    call_user_func('f2', 1);
  } catch (Error $ex) {
    echo "{$ex->getCode()}: {$ex->getMessage()} - {$ex->getFile()}({$ex->getLine()})\n\n";
  }
  try {
    call_user_func('f2');
  } catch (Error $ex) {
    echo "{$ex->getCode()}: {$ex->getMessage()} - {$ex->getFile()}({$ex->getLine()})\n\n";
  }
  try {
    call_user_func('f2', new A);
  } catch (Error $ex) {
    echo "{$ex->getCode()}: {$ex->getMessage()} - {$ex->getFile()}({$ex->getLine()})\n\n";
  }
  try {
    call_user_func('f2', null);
  } catch (Error $ex) {
    echo "{$ex->getCode()}: {$ex->getMessage()} - {$ex->getFile()}({$ex->getLine()})\n\n";
  }

  echo "\n\n---> Type hints with callback static method:\n";
  class C {
      static function f1(A $a) {
          if (isset($this)) {
              echo "in C::f1 (instance);\n";
          } else {
              echo "in C::f1 (static);\n";
          }
      }
      static function f2(A $a = null) {
          if (isset($this)) {
              echo "in C::f2 (instance);\n";
          } else {
              echo "in C::f2 (static);\n";
          }
      }
  }

  try {
    call_user_func(array('C', 'f1'), 1);
  } catch (Error $ex) {
    echo "{$ex->getCode()}: {$ex->getMessage()} - {$ex->getFile()}({$ex->getLine()})\n\n";
  }
  try {
    call_user_func(array('C', 'f1'), new A);
  } catch (Error $ex) {
    echo "{$ex->getCode()}: {$ex->getMessage()} - {$ex->getFile()}({$ex->getLine()})\n\n";
  }
  try {
    call_user_func(array('C', 'f2'), 1);
  } catch (Error $ex) {
    echo "{$ex->getCode()}: {$ex->getMessage()} - {$ex->getFile()}({$ex->getLine()})\n\n";
  }
  try {
    call_user_func(array('C', 'f2'));
  } catch (Error $ex) {
    echo "{$ex->getCode()}: {$ex->getMessage()} - {$ex->getFile()}({$ex->getLine()})\n\n";
  }
  try {
    call_user_func(array('C', 'f2'), new A);
  } catch (Error $ex) {
    echo "{$ex->getCode()}: {$ex->getMessage()} - {$ex->getFile()}({$ex->getLine()})\n\n";
  }
  try {
    call_user_func(array('C', 'f2'), null);
  } catch (Error $ex) {
    echo "{$ex->getCode()}: {$ex->getMessage()} - {$ex->getFile()}({$ex->getLine()})\n\n";
  }

  echo "\n\n---> Type hints with callback instance method:\n";
  class D {
      function f1(A $a) {
          if (isset($this)) {
              echo "in C::f1 (instance);\n";
          } else {
              echo "in C::f1 (static);\n";
          }
      }
      function f2(A $a = null) {
          if (isset($this)) {
              echo "in C::f2 (instance);\n";
          } else {
              echo "in C::f2 (static);\n";
          }
      }
  }
  $d = new D;

  try {
    call_user_func(array($d, 'f1'), 1);
  } catch (Error $ex) {
    echo "{$ex->getCode()}: {$ex->getMessage()} - {$ex->getFile()}({$ex->getLine()})\n\n";
  }
  try {
    call_user_func(array($d, 'f1'), new A);
  } catch (Error $ex) {
    echo "{$ex->getCode()}: {$ex->getMessage()} - {$ex->getFile()}({$ex->getLine()})\n\n";
  }
  try {
    call_user_func(array($d, 'f2'), 1);
  } catch (Error $ex) {
    echo "{$ex->getCode()}: {$ex->getMessage()} - {$ex->getFile()}({$ex->getLine()})\n\n";
  }
  try {
    call_user_func(array($d, 'f2'));
  } catch (Error $ex) {
    echo "{$ex->getCode()}: {$ex->getMessage()} - {$ex->getFile()}({$ex->getLine()})\n\n";
  }
  try {
    call_user_func(array($d, 'f2'), new A);
  } catch (Error $ex) {
    echo "{$ex->getCode()}: {$ex->getMessage()} - {$ex->getFile()}({$ex->getLine()})\n\n";
  }
  try {
    call_user_func(array($d, 'f2'), null);
  } catch (Error $ex) {
    echo "{$ex->getCode()}: {$ex->getMessage()} - {$ex->getFile()}({$ex->getLine()})\n\n";
  }

?>
--EXPECTF--
---> Type hints with callback function:
0: f1(): Argument #1 ($a) must be of type A, int given%s(%d)

in f1;
0: f2(): Argument #1 ($a) must be of type ?A, int given%s(%d)

in f2;
in f2;
in f2;


---> Type hints with callback static method:
0: C::f1(): Argument #1 ($a) must be of type A, int given%s(%d)

in C::f1 (static);
0: C::f2(): Argument #1 ($a) must be of type ?A, int given%s(%d)

in C::f2 (static);
in C::f2 (static);
in C::f2 (static);


---> Type hints with callback instance method:
0: D::f1(): Argument #1 ($a) must be of type A, int given%s(%d)

in C::f1 (instance);
0: D::f2(): Argument #1 ($a) must be of type ?A, int given%s(%d)

in C::f2 (instance);
in C::f2 (instance);
in C::f2 (instance);
