--TEST--
GH-25705 (session_reset() destroys $_SESSION while PS(http_session_vars) still points to it)
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
class Unset_ {
    public function __destruct() {
        var_dump($_SESSION);
        session_unset();
    }
}
class Destroy {
    public function __destruct() {
        session_destroy();
    }
}

ob_start();
session_start();
$_SESSION['boom'] = new Unset_();
$_SESSION['filler'] = str_repeat('A', 128);
$_SESSION['filler2'] = str_repeat('B', 128);
session_reset();
var_dump(session_status() === PHP_SESSION_ACTIVE, $_SESSION);

$_SESSION['boom'] = new Destroy();
$_SESSION['filler'] = str_repeat('A', 128);
session_reset();
var_dump(session_status() === PHP_SESSION_NONE);
echo "Done\n";
?>
--EXPECT--
array(0) {
}
bool(true)
array(0) {
}
bool(true)
Done
