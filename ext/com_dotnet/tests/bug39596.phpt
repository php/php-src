--TEST--
Bug #39596 (Creating Variant of type VT_ARRAY)
--EXTENSIONS--
com_dotnet
--FILE--
<?php
error_reporting(E_ALL);

try {
    $binding_string = array('aaa','bbb','ccc');
    $v = new VARIANT( $binding_string, VT_ARRAY );
    foreach ($v AS $element) {
        print $element."\n";
    }
} catch (Exception $e) {
    print $e;
}
?>
--EXPECT--
aaa
bbb
ccc
