--TEST--
session_unset(): destructors of old values do not observe $_SESSION being cleaned
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_cookies=0
session.cache_limiter=
session.save_handler=files
--FILE--
<?php
class Grow {
    public function __destruct() {
        for ($i = 0; $i < 64; $i++) {
            $_SESSION["k$i"] = str_repeat('x', 30);
        }
    }
}
class Reenter {
    public function __destruct() {
        var_dump(count($_SESSION));
        session_unset();
    }
}

ob_start();
session_start();

$_SESSION['a'] = new Grow();
$_SESSION['b'] = str_repeat('x', 100);
session_unset();
var_dump(count($_SESSION));

$_SESSION = ['a' => new Reenter(), 'b' => str_repeat('x', 100)];
session_unset();
var_dump($_SESSION);

session_destroy();
?>
--EXPECT--
int(64)
int(0)
array(0) {
}
