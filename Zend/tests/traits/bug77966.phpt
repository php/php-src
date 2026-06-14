--TEST--
Bug #77966: Cannot alias a method named "namespace"
--FILE--
<?php

trait A {
    function namespace() {
        echo "Called\n";
    }
}

class C {
    use A {
        namespace as bar;
    }
}

$c = new C;
$c->bar();
$c->namespace();

?>
--EXPECT--
Called
Called
