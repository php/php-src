--TEST--
Bug #78609 (mb_check_encoding() no longer supports stringable objects)
--EXTENSIONS--
mbstring
--FILE--
<?php
class Foo
{
    public function __toString()
    {
        return 'string_representation';
    }
}

var_dump(mb_check_encoding(new Foo, 'UTF-8'));
?>
--EXPECT--
bool(true)
