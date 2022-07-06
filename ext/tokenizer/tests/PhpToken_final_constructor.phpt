--TEST--
Check that the PhpToken constructor is final
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip tokenizer extension not enabled"; ?>
--FILE--
<?php

class MyPhpToken extends PhpToken {
    public function __construct() {
    }
}

?>
--EXPECTF--
Fatal error: Cannot override final method PhpToken::__construct() in %s on line %d
