--TEST--
Bug #81429 SplFixedArray::setSize(0) called during offsetSet()
--FILE--
<?php
class HasDestructor {
    public function __destruct() {
        $GLOBALS['values']->setSize(0);
    }
}

$values = new SplFixedArray(1);
$values->offsetSet(0, new HasDestructor());
$values->offsetSet(0, null);
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Index invalid or out of range in %s.php:%d
Stack trace:
#0 %s.php(%d): SplFixedArray->offsetSet(0, NULL)
#1 {main}
  thrown in %s.php on line %d
