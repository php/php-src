--TEST--
Extending the PhpToken class
--FILE--
<?php

$code = <<<'PHP'
<?PHP
FUNCTION FOO() {
    ECHO "bar";
}
PHP;

class MyPhpToken extends PhpToken {
    public function getLoweredText(): string {
        return strtolower($this->text);
    }
}

foreach (MyPhpToken::getAll($code) as $token) {
    echo $token->getLoweredText();
}

?>
--EXPECT--
<?php
function foo() {
    echo "bar";
}
