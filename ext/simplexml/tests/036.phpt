--TEST--
SimpleXML: overridden count() method
--EXTENSIONS--
simplexml
--FILE--
<?php
class SXE extends SimpleXmlElement {
    public function count(): int {
        echo "Called Count!\n";
        return parent::count();
    }
}

$str = '<xml><c>asdf</c><c>ghjk</c></xml>';
$sxe = new SXE($str);
var_dump(count($sxe));
?>
--EXPECT--
Called Count!
int(2)
