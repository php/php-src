--TEST--
Bug #40036 (empty() does not work correctly with ArrayObject when using ARRAY_AS_PROPS)
--FILE--
<?php
class View extends ArrayObject
{
    public function __construct(array $array = array())
    {
        parent::__construct($array, ArrayObject::ARRAY_AS_PROPS);
    }
}

$view = new View();
$view->foo = false;
$view->bar = null;
$view->baz = '';
if (empty($view['foo']) || empty($view->foo)) {
    echo "View::foo empty\n";
}
if (empty($view['bar']) || empty($view->bar)) {
    echo "View::bar empty\n";
}
if (empty($view['baz']) || empty($view->baz)) {
    echo "View::baz empty\n";
}
?>
--EXPECT--
View::foo empty
View::bar empty
View::baz empty
