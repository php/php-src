--TEST--
circular inheritance
--SKIPIF--
<?php
if (!extension_loaded('opcache')) {
    die('skip opcache required');
}
?>
--FILE--
<?php

class Outer {
 class Middle extends Outer\Other {
     class Inner1 extends Inner2 {}
     class Inner2 extends Other {}
 }
 abstract class Other {}
}
?>
--EXPECT--
