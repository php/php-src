--TEST--
Test stream_get_meta_data() function : error conditions
--FILE--
<?php
echo "*** Testing stream_get_meta_data() : error conditions ***\n";

echo "\n-- Testing stream_get_meta_data() function with closed stream resource --\n";
$fp = fopen(__FILE__, 'r');
fclose($fp);
try {
    var_dump(stream_get_meta_data($fp));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done";
?>
--EXPECT--
*** Testing stream_get_meta_data() : error conditions ***

-- Testing stream_get_meta_data() function with closed stream resource --
stream_get_meta_data(): supplied resource is not a valid stream resource
Done
