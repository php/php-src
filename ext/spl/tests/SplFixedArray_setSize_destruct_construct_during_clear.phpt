--TEST--
SplFixedArray::setSize: __construct() re-entrantly during clear (setSize(0))
--FILE--
<?php
class Reentrant {
    public ?SplFixedArray $arr = null;
    public function __destruct() {
        if ($this->arr !== null) {
            /* Re-initialising to an empty array allocates nothing, but still
             * resets the internal "resize pending" sentinel. */
            $this->arr->__construct(0);
        }
    }
}

$arr = new SplFixedArray(2);
$r = new Reentrant();
$r->arr = $arr;
$arr[0] = $r;
unset($r);
$arr[1] = "tail";

$arr->setSize(0);
echo "size: ", $arr->getSize(), "\n";

/* The array must still be usable afterwards. */
$arr->setSize(1);
$arr[0] = "ok";
var_dump($arr[0]);
?>
--EXPECT--
size: 0
string(2) "ok"
