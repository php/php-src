--TEST--
GH-24579 (Crash in FILTER_VALIDATE_INT when __toString() mutates the referenced options array)
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
var_dump($shared);

?>
--EXPECT--
int(1)
string(7) "changed"
