<?php

class Foo2
{
    function dummy(): void
    {
    }
}

class BadClass2 extends Foo2
{
    function dummy()
    {
    }
}
