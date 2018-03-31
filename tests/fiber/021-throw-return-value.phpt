--TEST--
tests Fiber for throw exception
--FILE--
<?php
$f = new Fiber(function () {
    try {
        $a = Fiber::yield();
    } catch (Exception $e) {
        return $e->getMessage();
    }
});
$f->resume();
echo $f->throw(new Exception("foo"));
?>
--EXPECTF--
foo
