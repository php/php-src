--TEST--
Ensure type hints are enforced for functions invoked as callbacks.
--FILE--
<?php
  set_error_handler('myErrorHandler', E_RECOVERABLE_ERROR);
  function myErrorHandler($errno, $errstr, $errfile, $errline) {
      echo "$errno: $errstr - $errfile($errline)\n";
      return true;
  }
  
  echo "---> Type hints with callback function:\n";
  class A  {  }
  function f1(A $a)  {
      echo "in f1;\n";
  }
  function f2(A $a = null)  {
      echo "in f2;\n";
  }
  call_user_func('f1', 1);
  call_user_func('f1', new A);
  call_user_func('f2', 1);
  call_user_func('f2');
  call_user_func('f2', new A);
  call_user_func('f2', null);
  
  
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
  call_user_func(array('C', 'f1'), 1);
  call_user_func(array('C', 'f1'), new A);
  call_user_func(array('C', 'f2'), 1);
  call_user_func(array('C', 'f2'));
  call_user_func(array('C', 'f2'), new A);
  call_user_func(array('C', 'f2'), null);
  
  
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
  call_user_func(array($d, 'f1'), 1);
  call_user_func(array($d, 'f1'), new A);
  call_user_func(array($d, 'f2'), 1);
  call_user_func(array($d, 'f2'));
  call_user_func(array($d, 'f2'), new A);
  call_user_func(array($d, 'f2'), null);
  
?>
--EXPECTF--
---> Type hints with callback function:
4096: Argument 1 passed to f1() must be an instance of A, integer given%s(10)
in f1;
in f1;
4096: Argument 1 passed to f2() must be an instance of A, integer given%s(13)
in f2;
in f2;
in f2;
in f2;


---> Type hints with callback static method:
4096: Argument 1 passed to C::f1() must be an instance of A, integer given%s(26)
in C::f1 (static);
in C::f1 (static);
4096: Argument 1 passed to C::f2() must be an instance of A, integer given%s(33)
in C::f2 (static);
in C::f2 (static);
in C::f2 (static);
in C::f2 (static);


---> Type hints with callback instance method:
4096: Argument 1 passed to D::f1() must be an instance of A, integer given%s(51)
in C::f1 (instance);
in C::f1 (instance);
4096: Argument 1 passed to D::f2() must be an instance of A, integer given%s(58)
in C::f2 (instance);
in C::f2 (instance);
in C::f2 (instance);
in C::f2 (instance);
