--TEST--
GH-25706 (session_decode() with php_serialize: use-after-free when a value added by __unserialize() has a destructor)
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_cookies=0
session.cache_limiter=
session.serialize_handler=php_serialize
session.save_handler=files
--FILE--
<?php
class Reentry {
    public function __destruct() {
        $GLOBALS['keep'] = $_SESSION;
    }
}
class Boom {
    public function __unserialize($data) {
        $_SESSION['pad'] = str_repeat('Z', 128);
        $_SESSION['re'] = new Reentry();
    }
}
ob_start();
session_start();
session_decode(serialize([new Boom()]));
var_dump($GLOBALS['keep']);
session_destroy();
?>
--EXPECT--
array(1) {
  [0]=>
  object(Boom)#1 (0) {
  }
}
