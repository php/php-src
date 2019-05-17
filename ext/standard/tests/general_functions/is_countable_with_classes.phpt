--TEST--
Test is_countable() function
--CREDITS--
Gabriel Caruso (carusogabriel34@gmail.com)
--FILE--
<?php
var_dump(is_countable(new class extends ArrayIterator {}));
var_dump(is_countable((array) new stdClass()));
var_dump(is_countable(new class implements Countable {
    public function count()
    {
        return count(1, 'foo');
    }
}));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
