--TEST--
Bug #55578 (Segfault on implode/concat)
--FILE--
<?php  
$options = array();

class Foo {
    public function __toString() {
        return 'Foo';
    }
}   

function test($options, $queryPart) {
	return ''. (0 ? 1 : $queryPart);
}

var_dump(test($options, new Foo()));
?>
--EXPECT--
string(3) "Foo"
