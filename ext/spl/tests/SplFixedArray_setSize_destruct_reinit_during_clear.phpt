--TEST--
SplFixedArray::setSize: re-initialising from a destructor during clear (GH-23811)
--DESCRIPTION--
setSize(0) clears elements and size before running the element destructors, so
the array momentarily looks like it was never constructed. __construct(),
__wakeup() and __unserialize() must not re-initialise it in that window: the
in-progress clear would discard whatever they installed, leaking it.
--FILE--
<?php
class Reentrant {
    public static $arr = null;
    public static $action = null;
    public function __destruct() {
        if (self::$action === null) {
            return;
        }
        $fn = self::$action;
        self::$action = null;
        $fn(self::$arr);
    }
}

function clear_with(callable $action): void {
    $arr = new SplFixedArray(2);
    $arr[0] = new Reentrant();
    $arr[1] = "tail";
    Reentrant::$arr = $arr;
    Reentrant::$action = $action;

    $arr->setSize(0);
    echo "size: ", $arr->getSize(), "\n";

    /* The array must still be usable. */
    $arr->setSize(1);
    $arr[0] = "ok";
    var_dump($arr[0]);

    Reentrant::$arr = null;
    Reentrant::$action = null;
}

echo "-- __construct() --\n";
clear_with(function ($arr) { $arr->__construct(5); });

/* __construct() is ignored, but the following setSize() is still recorded as
 * the pending resize and applied once the clear finishes. */
echo "-- __construct() then setSize() --\n";
clear_with(function ($arr) { $arr->__construct(7); $arr->setSize(3); });

echo "-- __unserialize() --\n";
clear_with(function ($arr) { $arr->__unserialize(["a", "b", "c"]); });

echo "-- __wakeup() --\n";
clear_with(function ($arr) { @$arr->__wakeup(); });
?>
--EXPECT--
-- __construct() --
size: 0
string(2) "ok"
-- __construct() then setSize() --
size: 3
string(2) "ok"
-- __unserialize() --
size: 0
string(2) "ok"
-- __wakeup() --
size: 0
string(2) "ok"
