--TEST--
Verifies the correct conversion of objects to arrays
--FILE--
<?php
class foo
{
    private $private = 'private';
    protected $protected = 'protected';
    public $public = 'public';
}
var_export((array) new foo);
?>
--EXPECT--
array (
  "\000foo\000private" => 'private',
  "\000*\000protected" => 'protected',
  'public' => 'public',
)
