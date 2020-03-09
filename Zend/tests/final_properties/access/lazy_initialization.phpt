--TEST--
Test that final properties can be initialized lazily by unsetting them and using __get()
--FILE--
<?php

class Foo
{
    final public int $property1;
    final public string $property2;
    final public stdClass $property3;
}

class Bar extends Foo
{
    private $lazyProperty1;
    private $lazyProperty2;
    private $lazyProperty3;

    public function __construct()
    {
        unset($this->property1);
        unset($this->property2);
        unset($this->property3);
    }

    public function __get($name)
    {
        switch ($name) {
            case "property1":
                if ($this->lazyProperty1 === null) {
                    $this->lazyProperty1 = 1;
                }

                return $this->lazyProperty1;
            case "property2":
                if ($this->lazyProperty2 === null) {
                    $this->lazyProperty2 = "foo";
                }

                return $this->lazyProperty2;
            case "property3":
                 if ($this->lazyProperty3 === null) {
                     $this->lazyProperty3 = new stdClass();
                 }

                 return $this->lazyProperty3;
            default:
                throw new Exception();
        }
    }
}

$foo = new Bar();

var_dump($foo->property1);
var_dump($foo->property2);
var_dump($foo->property3);

?>
--EXPECT--
int(1)
string(3) "foo"
object(stdClass)#2 (0) {
}
