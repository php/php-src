--TEST--
Destructors of session values do not observe the session array being released
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
class EncodeOnDestruct {
    public function __destruct() {
        var_dump(session_encode());
    }
}
class Boom {
    public function __unserialize($data) {
        $_SESSION['f'] = str_repeat('A', 128);
        $_SESSION['re'] = new EncodeOnDestruct();
        unset($GLOBALS['_SESSION']);
    }
}

ob_start();

echo "session_destroy():\n";
session_start();
$_SESSION['f'] = str_repeat('A', 128);
$_SESSION['boom'] = new EncodeOnDestruct();
unset($_SESSION);
session_destroy();

echo "session_decode() with php_serialize:\n";
ini_set('session.serialize_handler', 'php_serialize');
session_start();
session_decode(serialize([new Boom()]));
session_destroy();
echo "Done\n";
?>
--EXPECTF--
session_destroy():

Warning: session_encode(): Cannot encode non-existent session in %s on line %d
bool(false)
session_decode() with php_serialize:
string(25) "a:1:{i:0;O:4:"Boom":0:{}}"
Done
