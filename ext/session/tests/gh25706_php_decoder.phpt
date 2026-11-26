--TEST--
GH-25706 (session_decode() with the php decoder: destructor of a replaced value modifies $_SESSION)
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_cookies=0
session.cache_limiter=
session.serialize_handler=php
session.save_handler=files
--FILE--
<?php
class UnsetOnDestruct {
    public function __destruct() {
        session_unset();
    }
}
class ReplaceOnDestruct {
    public function __destruct() {
        $_SESSION = [];
    }
}

ob_start();
session_start();

$_SESSION['a'] = new UnsetOnDestruct();
$_SESSION['pad'] = str_repeat('A', 64);
session_decode('a|i:1;b|i:2;c|i:3;');
var_dump($_SESSION);

$_SESSION['a'] = new ReplaceOnDestruct();
$_SESSION['pad'] = str_repeat('A', 64);
session_decode('a|i:1;b|i:2;c|i:3;');
var_dump($_SESSION);

session_destroy();
?>
--EXPECT--
array(2) {
  ["b"]=>
  int(2)
  ["c"]=>
  int(3)
}
array(2) {
  ["b"]=>
  int(2)
  ["c"]=>
  int(3)
}
