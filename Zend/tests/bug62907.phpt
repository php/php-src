--TEST--
Bug #62907 (Double free when use traits)
--FILE--
<?php

spl_autoload_register(function ($name) {
    if ($name == "B") {
        eval("abstract class B extends A { }");
    } elseif ($name == "A") {
        eval("abstract class A { use T { T::__construct as __asconstruct; }}");
    } elseif ($name == "T") {
        eval("trait T { public function __construct() { } }");
    }
    return true;
});

class C extends B
{
    public function __construct()
    {
    }
}
echo "okey";

?>
--EXPECT--
okey
