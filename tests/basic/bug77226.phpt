--TEST--
Bug #77226 (convert object which has invisible properties to array)
--FILE--
<?php
$array = new class
{
    private $private = 1;
    protected $protected = 2;
    public $public = 3;
};
$array = (array)$array;
var_dump($array);
assert(count($array) === 1);
assert(!isset($array['private']));
assert(!isset($array['protected']));
assert($array['public'] === 3);

class Test
{
    private $private = 1;
    protected $protected = 2;
    public $public = 3;
}

$array = (array)new Test;
var_dump($array);
assert(count($array) === 1);
assert(!isset($array['private']));
assert(!isset($array['protected']));
assert($array['public'] === 3);

class TestEx extends Test
{
    private $private_ex = 1;
    protected $protected_ex = 2;
    public $public_ex = 3;
}

$array = (array)new TestEx;
var_dump($array);
assert(count($array) === 2);
assert(!isset($array['private']));
assert(!isset($array['protected']));
assert($array['public'] === 3);
assert(!isset($array['private_ex']));
assert(!isset($array['protected_ex']));
assert($array['public_ex'] === 3);

class TestOver extends Test
{
    private $private = 4;
    protected $protected = 5;
    public $public = 6;
}

$array = (array)new TestOver;
var_dump($array);
assert(count($array) === 1);
assert(!isset($array['private']));
assert(!isset($array['protected']));
assert($array['public'] === 6);

class TestOverOver extends TestOver
{
    public $private = 7;
    public $protected = 8;
    public $public = 9;
    public $private_ex = 10;
    public $protected_ex = 11;
    public $public_ex = 12;
}

$array = (array)new TestOverOver;
var_dump($array);
assert(count($array) === 6);
assert($array['private'] === 7);
assert($array['protected'] === 8);
assert($array['public'] === 9);
assert($array['private_ex'] === 10);
assert($array['protected_ex'] === 11);
assert($array['public_ex'] === 12);

echo "Done\n";
?>
--EXPECT--
array(1) {
  ["public"]=>
  int(3)
}
array(1) {
  ["public"]=>
  int(3)
}
array(2) {
  ["public_ex"]=>
  int(3)
  ["public"]=>
  int(3)
}
array(1) {
  ["public"]=>
  int(6)
}
array(6) {
  ["private"]=>
  int(7)
  ["protected"]=>
  int(8)
  ["public"]=>
  int(9)
  ["private_ex"]=>
  int(10)
  ["protected_ex"]=>
  int(11)
  ["public_ex"]=>
  int(12)
}
Done
