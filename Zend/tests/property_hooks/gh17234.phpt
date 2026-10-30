--TEST--
GH-17234 (Numeric parent hook call fails with assertion)
--FILE--
<?php
class ParentC {}
class Child extends ParentC {
    public $a {
        get {
            return parent::${0}::get ();
        }
    }
}
?>
--EXPECTF--
Fatal error: Must not use parent::$0::get() in a different property ($a) in %s on line %d
