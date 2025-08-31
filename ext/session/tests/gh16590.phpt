--TEST--
GH-16590 (UAF in session_encode())
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

class C {
    function __serialize() {
        $_SESSION = [];
        return [];
    }
}

session_start();

$_SESSION['Lz'] = new C;
for ($i = 0; $i < 2; $i++) {
    $_SESSION[$i] = $i;
}

var_dump(session_encode());

?>
--EXPECTF--
Warning: session_encode(): Skipping numeric key 0 in %s on line %d

Warning: session_encode(): Skipping numeric key 1 in %s on line %d
string(15) "Lz|O:1:"C":0:{}"
