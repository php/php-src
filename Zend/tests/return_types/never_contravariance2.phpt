--TEST--
never type: non-explicit widening allowed
--FILE--
<?php

class A
{
    public function someReturningStaticMethod(never $var)
    {
    }
}

class B extends A
{
    public function someReturningStaticMethod($var)
    {
        echo "success";
    }
}

(new B)->someReturningStaticMethod("test");
?>
--EXPECT--
success
