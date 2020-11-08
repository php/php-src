--TEST--
WeakReference object handlers
--FILE--
<?php
$wr = WeakReference::create(new stdClass);

var_dump($wr->disallow);
var_dump(isset($wr->disallow));
unset($wr->disallow);

try {
    $wr->disallow = "writes";
} catch (Error $ex) {
    var_dump($ex->getMessage());
}

try {
    $disallow = &$wr->disallowed;
} catch (Error $ex) {
    var_dump($ex->getMessage());
}
?>
--EXPECTF--
Warning: Undefined property: WeakReference::$disallow in %s on line %d
NULL
bool(false)
string(55) "Cannot create dynamic property WeakReference::$disallow"
string(57) "Cannot create dynamic property WeakReference::$disallowed"
