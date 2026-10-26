--TEST--
GH-24674 (Destructor run by session_decode() gets its own unserialize() context)
--EXTENSIONS--
session
--INI--
session.use_cookies=0
session.cache_limiter=
session.serialize_handler=php
session.save_handler=files
--FILE--
<?php
class D {
    public function __destruct() {
        echo "destructor: ";
        var_dump(@unserialize('R:1;'));
    }
}

session_start();
$_SESSION['a'] = new D;
session_decode('b|s:6:"secret";a|i:5;');
var_dump($_SESSION);
session_abort();
?>
--EXPECT--
destructor: bool(false)
array(2) {
  ["a"]=>
  int(5)
  ["b"]=>
  string(6) "secret"
}
