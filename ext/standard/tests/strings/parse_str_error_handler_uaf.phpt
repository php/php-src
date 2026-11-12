--TEST--
parse_str() remains safe when its error handler replaces the result array
--INI--
display_errors=0
max_input_nesting_level=1
--FILE--
<?php
$result = null;
$handled = 0;

set_error_handler(function (int $errno, string $message) use (&$result, &$handled): bool {
    if ($errno === E_WARNING
        && strpos($message, 'Input variable nesting level exceeded') !== false) {
        $handled++;
        $result = null;
        return true;
    }

    return false;
});

parse_str('a[b][c]=1&later=2', $result);
restore_error_handler();

var_dump($handled);
echo "done\n";
?>
--EXPECT--
int(1)
done
