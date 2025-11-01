--TEST--
GH-20286 use after destroy on userland stream_close
--CREDITS--
vi3tL0u1s
--SKIPIF--
<?php
if (PHP_DEBUG) die('skip requires release build');
?>
--FILE--
<?php
class lib {
   public $context;
   function stream_set() {}
   function stream_set_option() {}
   function stream_stat() {
     return true;
   }
   function stream_open() {
     return true;
   }

   function stream_read($count) {
     function a() {}
     include('lib://');
   }

   function stream_close() {
     include('lib://');
   }
}
stream_wrapper_register('lib', lib::class);
include('lib://test.php');
?>
--EXPECTF--

Fatal error: Cannot redeclare a() (previously declared in %s:%d) in %s on line %d

Fatal error: Cannot redeclare a() (previously declared in %s:%d) in %s on line %d

Fatal error: Cannot redeclare a() (previously declared in %s:%d) in %s on line %d
