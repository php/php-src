--TEST--
GH-24579: FILTER_VALIDATE_INT should not crash when __toString() mutates options array
--EXTENSIONS--
filter
--FILE--
<?php

$shared = ['min_range' => 0];
$args = ['options' => &$shared];

class Reenter
{
    public function __toString(): string
    {
        global $shared;
        $shared = 'changed';
        return '1';
    }
}

var_dump(filter_var(new Reenter(), FILTER_VALIDATE_INT, $args));

echo "No crash\n";
?>
--EXPECT--
int(1)
No crash
