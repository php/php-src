--TEST--
<?= cannot be used as an identifier
--FILE--
<?php
trait T {
    public function x() {}
}
class C {
    use T {
        x as y?><?= as my_echo;
    }
}
?>
--EXPECTF--
Parse error: Cannot use "<?=" as an identifier in %s on line %d
