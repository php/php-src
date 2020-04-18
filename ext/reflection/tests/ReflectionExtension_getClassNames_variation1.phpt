--TEST--
ReflectionExtension::getClassNames() method on an extension with no classes
--CREDITS--
Felix De Vliegher <felix.devliegher@gmail.com>
--SKIPIF--
<?php
extension_loaded('ctype') or die("skip Requires 'ctype' extension");
?>
--FILE--
<?php
$extension = new ReflectionExtension('ctype');
var_dump($extension->getClassNames());
?>
--EXPECT--
array(0) {
}
