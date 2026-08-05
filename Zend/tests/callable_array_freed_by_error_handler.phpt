--TEST--
Array callable survives an error handler that frees it during validation
--FILE--
<?php
class Victim {
    public $tag = "alive";
    public function instanceTarget() { echo "instance on ", $this->tag, "\n"; }
    public static function staticTarget() { echo "static\n"; }
}
class Holder extends Victim {}

set_error_handler(function ($errno, $errstr) {
    if (str_contains($errstr, 'Callables of the form')) {
        $GLOBALS['cb'] = null;
        gc_collect_cycles();
    }
    return true;
});

$cb = [new Holder(), 'Victim::instanceTarget'];
call_user_func($cb);

$cb = [new Holder(), 'Victim::staticTarget'];
call_user_func($cb);

echo "done\n";
?>
--EXPECT--
instance on alive
static
done
