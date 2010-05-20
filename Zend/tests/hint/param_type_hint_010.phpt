--TEST--
Parameter type hint - Testing with heredoc
--FILE--
<?php

function test(string $str) {
}

test(<<<FOO
test!
FOO
);

test(<<<'FOO'
test!
FOO
);

test(<<<"FOO"
test!
FOO
);

print "Ok!";

?>
--EXPECT--
Ok!
