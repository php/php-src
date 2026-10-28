--TEST--
GH-24802 (request_parse_body() reentrancy double-frees protected variables)
--INI--
display_errors=0
--ENV--
REQUEST_METHOD=PUT
--POST_RAW--
Content-Type: multipart/form-data; boundary=----X
------X
Content-Disposition: form-data; name="a[x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x][x]"

val
------X--
--FILE--
<?php
set_error_handler(function ($no, $str) {
    static $done = false;
    if (!$done) {
        $done = true;
        try {
            request_parse_body();
        } catch (Throwable $e) {
            echo 'nested: ', get_class($e), ': ', $e->getMessage(), "\n";
        }
    }
    return true;
});

[$_POST, $_FILES] = request_parse_body();
echo "outer done\n";
var_dump($_POST, $_FILES);
?>
--EXPECT--
nested: RequestParseBodyException: Cannot call request_parse_body() while the request body is already being parsed
outer done
array(0) {
}
array(0) {
}
