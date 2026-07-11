--TEST--
GH-22061 (Double free via re-entrant destructor in zend_hash_clean via session_unset)
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
session_start();

class SelfRemover {
    public function __destruct() {
        unset($_SESSION['x']);
    }
}

$_SESSION['x'] = new SelfRemover();
session_unset();

echo "done\n";
?>
--EXPECT--
done
