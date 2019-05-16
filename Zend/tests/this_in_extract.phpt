--TEST--
$this re-assign in extract()
--FILE--
<?php
function foo() {
    try {
        extract(["this"=>42, "a"=>24]);
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
    var_dump($a);
}
foo();
?>
--EXPECTF--
Cannot re-assign $this

Notice: Undefined variable: a in %s on line %d
NULL
