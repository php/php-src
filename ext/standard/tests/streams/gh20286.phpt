--TEST--
GH-20286 use after destroy on userland stream_close
--CREDITS--
vi3tL0u1s
--FILE--
<?php
class lib {
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

Deprecated: Creation of dynamic property lib::$context is deprecated in %s on line %d

Warning: include(): lib::stream_set_option is not implemented! in %s on line %d

Warning: include(): lib::stream_stat is not implemented! in %s on line %d

Deprecated: Creation of dynamic property lib::$context is deprecated in %s on line %d

Warning: include(): lib::stream_set_option is not implemented! in %s on line %d

Warning: include(): lib::stream_stat is not implemented! in %s on line %d

Fatal error: Cannot redeclare a() (previously declared in %s:%d) %s on line %d

Deprecated: Creation of dynamic property lib::$context is deprecated in %s on line %d

Warning: include(): lib::stream_set_option is not implemented! in %s on line %d

Warning: include(): lib::stream_stat is not implemented! in %s on line %d

Fatal error: Cannot redeclare a() (previously declared in %s:%d) %s on line %d

Deprecated: Creation of dynamic property lib::$context is deprecated in %s on line %d

Warning: include(): lib::stream_set_option is not implemented! in %s on line %d

Warning: include(): lib::stream_stat is not implemented! in %s on line %d

Fatal error: Cannot redeclare a() (previously declared in %s:%d) %s on line %d
