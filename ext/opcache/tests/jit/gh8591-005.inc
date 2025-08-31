<?php

class AbstractModel
{
    protected static int $field = 1;

    final protected function cast()
    {
        global $x;
        $x = static::$field;
    }
}
