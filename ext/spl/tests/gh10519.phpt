--TEST--
Bug GH-10519 (Array Data Address Reference Issue)
--FILE--
<?php
interface DataInterface extends JsonSerializable, RecursiveIterator, Iterator
{
    public static function init(Traversable $data): DataInterface;
}

class A extends RecursiveArrayIterator implements DataInterface
{
    public function __construct($data)
    {
        parent::__construct($data);
    }

    public static function init($data): DataInterface
    {
        return new static($data);
    }

    public function getCols(string $colname): array
    {
        return array_column($this->getArrayCopy(), $colname);
    }

    public function bugySetMethod($key, $value)
    {
        $data = &$this;

        while ($data->hasChildren()) {
            $data = $data->getChildren();
        }
	    $data->offsetSet($key, $value);
    }

    public function jsonSerialize(): mixed
    {
        return $this;
    }
}

$a = [
    'test' => [
        'a' => (object) [2 => '',3 => '',4 => ''],
    ]
];

$example = A::init($a);
$example->bugySetMethod(5, 'in here');
var_dump(json_encode($example));
var_dump(json_encode($a));

$b = [
    'test' => [
        'b' => [2 => '',3 => '',4 => ''],
    ]
];
$example = A::init($b);

$example->bugySetMethod(5, 'must be here');
var_dump(json_encode($example));
var_dump(json_encode($b));
?>
--EXPECT--
string(51) "{"test":{"a":{"2":"","3":"","4":"","5":"in here"}}}"
string(51) "{"test":{"a":{"2":"","3":"","4":"","5":"in here"}}}"
string(56) "{"test":{"b":{"2":"","3":"","4":"","5":"must be here"}}}"
string(37) "{"test":{"b":{"2":"","3":"","4":""}}}"
