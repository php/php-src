--TEST--
$this re-assign in extract()
--FILE--
<?php
function foo() {
    try {
        extract(["this"=>42, "a"=>24]);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    var_dump($a);
}
foo();
?>
--EXPECTF--
Error: Cannot re-assign $this

Warning: Undefined variable $a in %s on line %d
NULL
