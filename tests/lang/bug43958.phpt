--TEST--
Bug #43958 (class name added into the error message)
--FILE--
<?php
class MyClass
{
    static public function loadCode($p) {
        return include $p;
    }
}

MyClass::loadCode('file-which-does-not-exist-on-purpose.php');
?>
--EXPECTF--
Warning: include(file-which-does-not-exist-on-purpose.php): Failed to open stream: No such file or directory in %sbug43958.php on line 5

Warning: include(): Failed opening 'file-which-does-not-exist-on-purpose.php' for inclusion (include_path='%s') in %sbug43958.php on line 5
