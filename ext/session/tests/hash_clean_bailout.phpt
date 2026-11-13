--TEST--
Use-after-free when a destructor bails out during zend_hash_clean() via session_unset()
--EXTENSIONS--
session
--XLEAK--
The intentional E_USER_ERROR bailout abandons the aborted request's op_array.
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.save_handler=files
session.use_cookies=0
session.cache_limiter=
--FILE--
<?php
error_reporting(E_ALL & ~E_DEPRECATED);
class Dtor {
    public function __destruct() {}
}
class Killer {
    public function __destruct() {
        trigger_error("boom", E_USER_ERROR);
    }
}
session_start();
$_SESSION['a'] = new Dtor();
$_SESSION['b'] = new Killer();
$_SESSION['c'] = new Dtor();
session_unset();
?>
--EXPECTF--
Fatal error: boom in %s on line %d
