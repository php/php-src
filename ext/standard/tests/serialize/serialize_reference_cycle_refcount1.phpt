--TEST--
serialize(): array referring to itself through references of refcount 1
--FILE--
<?php

function build() {
    $parent = ['name' => 'parent'];
    $child = ['name' => 'child'];
    $child['parent'] = &$parent;
    $parent['children'][] = &$child;
    return $parent;
}

$serialized = serialize(build());
echo $serialized, "\n";
var_dump(unserialize($serialized)['children'][0]['parent']['children'][0]);

?>
--EXPECT--
a:2:{s:4:"name";s:6:"parent";s:8:"children";a:1:{i:0;a:2:{s:4:"name";s:5:"child";s:6:"parent";a:2:{s:4:"name";s:6:"parent";s:8:"children";a:1:{i:0;N;}}}}}
NULL
