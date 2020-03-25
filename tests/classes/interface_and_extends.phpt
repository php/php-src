--TEST--
ZE2 a class cannot extend an interface
--FILE--
<?php

interface Test
{
    function show();
}

class Tester extends Test
{
    function show() {
        echo __METHOD__ . "\n";
    }
}

$o = new Tester;
$o->show();

?>
===DONE===
--EXPECTF--
Fatal error: Class Tester cannot extend from interface Test in %sinterface_and_extends.php on line %d
