--TEST--
Inherited methods: substituted clone keeps the erased class-name view when the return type wraps a method-level type parameter
--FILE--
<?php
class Box<+T> {
    private mixed $value;
    public function __construct(mixed $val) { $this->value = $val; }
    public static function create<O>(O $value): Box<O> { return new self($value); }
}

class StringBox extends Box<string> {}

$b = StringBox::create("hello");
var_dump($b instanceof Box);
$rm = (new ReflectionMethod(StringBox::class, 'create'));
echo "return type: ", (string)$rm->getReturnType(), "\n";
?>
--EXPECT--
bool(true)
return type: Box
