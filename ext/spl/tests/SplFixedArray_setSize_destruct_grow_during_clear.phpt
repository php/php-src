--TEST--
SplFixedArray::setSize: grow re-entrantly during clear (setSize(0))
--FILE--
<?php
class Reentrant {
    public ?SplFixedArray $arr = null;
    public function __destruct() {
        if ($this->arr !== null) {
            $this->arr->setSize(5);
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
$arr[0] = "ok";
var_dump($arr[0]);
?>
--EXPECT--
size: 5
string(2) "ok"
