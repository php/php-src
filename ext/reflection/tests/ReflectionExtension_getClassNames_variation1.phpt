--TEST--
ReflectionExtension::getClassNames() method on an extension with no classes
--CREDITS--
Felix De Vliegher <felix.devliegher@gmail.com>
--FILE--
<?php
$ereg = new ReflectionExtension('ereg');
var_dump($ereg->getClassNames());
?>
==DONE==
--EXPECT--
array(0) {
}
==DONE==
