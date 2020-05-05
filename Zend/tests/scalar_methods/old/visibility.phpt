--TEST--
Visibility for handler methods is honored
--FILE--
<?php

use extension string StringHandler;

class StringHandler {
    public static function pub($self) {
        echo "Public\n";
        $self->prot();
    }
    protected static function prot($self) {
        echo "Protected\n";
    }
}

set_exception_handler(function($e) {
    echo "\nFatal error: {$e->getMessage()} in {$e->getFile()} on line {$e->getLine()}";
});

$str = "abc";
$str->pub();
$str->prot();

?>
--EXPECTF--
Public
Protected

Fatal error: Call to protected method StringHandler::prot() from context '' in %s on line %d
