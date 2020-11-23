--TEST--
Bug #76300: Unserialize of extended protected member broken
--FILE--
<?php
class Base {
    private $id;
    public function __construct($id)
    {
        $this->id = $id;
    }
}
class Derived extends Base {
    protected $id;
    public function __construct($id)
    {
        parent::__construct($id + 20);
        $this->id = $id;
    }
}
$a = new Derived(44);
$s = serialize($a);
$u = unserialize($s);
print_r($u);
?>
--EXPECT--
Derived Object
(
    [id:protected] => 44
    [id:Base:private] => 64
)
