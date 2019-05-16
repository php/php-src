--TEST--
Bug #46409 (__invoke method called outside of object context when using array_map)
--FILE--
<?php
class Callback {
    protected $val = 'hello, world';

    public function __invoke() {
        return $this->val;
    }
}

$cb = new Callback();
echo $cb(),"\n";
$a = array(1, 2);
$b = array_map($cb, $a);
print_r($b);
?>
--EXPECT--
hello, world
Array
(
    [0] => hello, world
    [1] => hello, world
)
