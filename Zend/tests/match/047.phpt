--TEST--
Match expression inside subject expression
--FILE--
<?php

echo match (match ('b') { default => 'b' }) {
    'a' => 100,
    'b' => 200,
    'c' => 300
};

?>
--EXPECT--
200
