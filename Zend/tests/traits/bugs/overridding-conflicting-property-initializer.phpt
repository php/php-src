--TEST--
Properties are considered incompatible if they are different in any of their
defined characteristics. Thus, initialization values have to be equal, too.
--FILE--
<?php
error_reporting(E_ALL);

trait foo
{
    public $zoo = 'foo::zoo';
}

class baz
{
    use foo;
    public $zoo = 'baz::zoo';
}

$obj = new baz();
echo $obj->zoo, "\n";
?>
--EXPECTF--
Fatal error: baz and foo define the same property ($zoo) in the composition of baz. However, the definition differs and is considered incompatible. Class was composed in %s on line %d
