--TEST--
Dump heredoc tags
--INI--
zend.assertions=1
--FILE--
<?php
try {
    assert(false &&
    <<<HTML
    val inside
    HTML
    );
} catch(\Throwable $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
assert(false && <<<HEREDOC
val inside
HEREDOC)
