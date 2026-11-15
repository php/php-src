--TEST--
ArrayObject: an error handler can replace the backing store during an offset access
--FILE--
<?php
function replace_in_error_handler(ArrayObject $ao): void
{
    set_error_handler(function (int $errno, string $errstr) use ($ao) {
        echo $errstr, "\n";
        $ao->exchangeArray(['replaced' => str_repeat('x', 3)]);
    });
}

echo "-- read\n";
$ao = new ArrayObject(['a' => 1, 'b' => 2]);
replace_in_error_handler($ao);
var_dump($ao[1.5]);
restore_error_handler();

echo "-- isset\n";
$ao = new ArrayObject(['a' => 1, 'b' => 2]);
replace_in_error_handler($ao);
var_dump(isset($ao[1.5]));
restore_error_handler();

echo "-- read-write of a missing key\n";
$ao = new ArrayObject(['a' => 1], ArrayObject::ARRAY_AS_PROPS);
replace_in_error_handler($ao);
$ao->b .= 'x';
restore_error_handler();
var_dump($ao->getArrayCopy());

echo "-- read-write of an unset property of a backing object\n";
class C {
    public $p;
}
$o = new C();
unset($o->p);
$ao = @new ArrayObject($o, ArrayObject::ARRAY_AS_PROPS);
unset($o);
replace_in_error_handler($ao);
$ao->p .= 'x';
restore_error_handler();
var_dump($ao->getArrayCopy());

echo "-- ArrayIterator::seek()\n";
$it = new ArrayIterator(['a', 'b']);
set_error_handler(function (int $errno, string $errstr) use ($it) {
    echo $errstr, "\n";
    $it->__construct(['x', 'y', 'z']);
});
$it->seek(1.5);
restore_error_handler();
var_dump($it->current());
?>
--EXPECT--
-- read
Implicit conversion from float 1.5 to int loses precision
Undefined array key 1
NULL
-- isset
Implicit conversion from float 1.5 to int loses precision
bool(false)
-- read-write of a missing key
Undefined array key "b"
array(2) {
  ["replaced"]=>
  string(3) "xxx"
  ["b"]=>
  string(1) "x"
}
-- read-write of an unset property of a backing object
Undefined array key "p"
array(2) {
  ["replaced"]=>
  string(3) "xxx"
  ["p"]=>
  string(1) "x"
}
-- ArrayIterator::seek()
Implicit conversion from float 1.5 to int loses precision
string(1) "y"
