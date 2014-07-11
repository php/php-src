--TEST--
Reflection Bug #36308 (ReflectionProperty::getDocComment() does not reflect extended class commentary)
--INI--
opcache.save_comments=1
opcache.load_comments=1
--FILE--
<?php
class Base {
    /** Base comment block */
    public $foo = 'bar';
}

class Extended extends Base {
    /** Extended commentary */
    public $foo = 'zim';
}

$reflect = new ReflectionClass('Extended');
$props = $reflect->getProperties();
echo $props[0]->getDocComment();
?>
--EXPECT--
/** Extended commentary */
