--TEST--
Array callable whose object is destroyed by an error handler during validation
--FILE--
<?php
class Victim {
    public $tag = "alive";
    public function instanceTarget() { echo "instance on ", $this->tag, "\n"; }
}
class Holder extends Victim {}

$destroy = false;
set_error_handler(function ($errno, $errstr) use (&$destroy) {
    if ($destroy && str_contains($errstr, 'Callables of the form')) {
        $GLOBALS['cb'] = null;
        $GLOBALS['obj'] = null;
        gc_collect_cycles();
    }
    return true;
});

$cb = [new Holder(), 'Victim::instanceTarget'];
call_user_func($cb);

$destroy = true;

$cb = [new Holder(), 'Victim::instanceTarget'];
try {
    call_user_func($cb);
} catch (TypeError $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

$obj = new Holder();
try {
    call_user_func([&$obj, 'Victim::instanceTarget']);
} catch (TypeError $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

$obj = new Holder();
try {
    array_map([&$obj, 'Victim::instanceTarget'], [1]);
} catch (TypeError $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

echo "done\n";
?>
--EXPECT--
instance on alive
TypeError: call_user_func(): Argument #1 ($callback) must be a valid callback, object was destroyed while resolving the callable
TypeError: call_user_func(): Argument #1 ($callback) must be a valid callback, object was destroyed while resolving the callable
TypeError: array_map(): Argument #1 ($callback) must be a valid callback or null, object was destroyed while resolving the callable
done
