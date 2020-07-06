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
Warning: Undefined property WeakReference::$disallow in %s on line %d
NULL
bool(false)
string(59) "Dynamic property WeakReference::$disallow cannot be created"
string(61) "Dynamic property WeakReference::$disallowed cannot be created"
