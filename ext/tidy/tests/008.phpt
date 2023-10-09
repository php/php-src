--TEST--
Accessing the error buffer via $obj->error_buf...
--EXTENSIONS--
tidy
--FILE--
<?php
    $a = tidy_parse_string("<HTML><asd asdf></HTML>");
    echo $a->errorBuffer;
?>
--EXPECTF--
line 1 column 1 - Warning: missing <!DOCTYPE> declaration
line 1 column 7 - Error: <asd> is not recogni%ced!
line 1 column 7 - Warning: discarding unexpected <asd>
line 1 column 17 - Warning: discarding unexpected </html>
line 1 column 7 - Warning: inserting missing 'title' element
