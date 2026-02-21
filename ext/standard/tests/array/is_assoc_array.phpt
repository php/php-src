--TEST--
Test is_assoc_array() behavior
--FILE--
<?php

echo "1. "; var_dump(is_assoc_array(['a' => 'a', 0 => 'b']));
echo "2. "; var_dump(is_assoc_array([1 => 'a', 0 => 'b']));
echo "3. "; var_dump(is_assoc_array([1 => 'a', 2 => 'b']));
echo "4. "; var_dump(is_assoc_array([0 => 'a', 1 => 'b']));
echo "5. "; var_dump(is_assoc_array(['a', 'b']));

echo "6. "; var_dump(is_assoc_array([]));
echo "7. "; var_dump(is_assoc_array([1, 2, 3]));
echo "8. "; var_dump(is_assoc_array(['foo', 2, 3]));
echo "9. "; var_dump(is_assoc_array([0 => 'foo', 'bar']));

echo "10. "; var_dump(is_assoc_array([1 => 'foo', 'bar']));
echo "11. "; var_dump(is_assoc_array([0 => 'foo', 'bar' => 'baz']));
echo "12. "; var_dump(is_assoc_array([0 => 'foo', 2 => 'bar']));
echo "13. "; var_dump(is_assoc_array(['foo' => 'bar', 'baz' => 'qux']));

?>
--EXPECT--
1. bool(true)
2. bool(true)
3. bool(true)
4. bool(false)
5. bool(false)
6. bool(false)
7. bool(false)
8. bool(false)
9. bool(false)
10. bool(true)
11. bool(true)
12. bool(true)
13. bool(true)
