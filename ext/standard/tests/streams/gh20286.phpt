--TEST--
GH-20286 use after destroy on userland stream_close
--CREDITS--
vi3tL0u1s
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') die('skip Aborts with STATUS_BAD_FUNCTION_TABLE on Windows');
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
     static $count = 0;
     if ($count++ < 3) // Prevent infinite loop
      include('lib://');
   }
}
stream_wrapper_register('lib', lib::class);
include('lib://test.php');
?>
--EXPECTF--
Fatal error: Cannot redeclare function a() (previously declared in %s:%d) in %s on line %d

Fatal error: Cannot redeclare function a() (previously declared in %s:%d) in %s on line %d

Fatal error: Cannot redeclare function a() (previously declared in %s:%d) in %s on line %d

Fatal error: Cannot redeclare function a() (previously declared in %s:%d) in %s on line %d
