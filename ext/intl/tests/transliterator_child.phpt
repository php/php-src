--TEST--
Subclass Transliterator
--EXTENSIONS--
intl
--FILE--
<?php
class ChildTransliterator extends Transliterator
{
    public readonly string $id;

    public static function new()
    {
        $tr = (new \ReflectionClass(ChildTransliterator::class))->newInstanceWithoutConstructor();
        $tr->id = 'abc';

        return $tr;
    }
}


$tr = ChildTransliterator::new();

var_dump($tr->id);
?>
--EXPECT--
string(3) "abc"
