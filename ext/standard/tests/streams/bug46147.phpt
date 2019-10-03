--TEST--
Bug #46147 (after stream seek, appending stream filter reads incorrect data)
--FILE--
<?php
$fp = tmpfile();
fwrite($fp, "this is a lowercase string.\n");
fseek($fp, 5);
stream_filter_append($fp, "string.toupper");
while (!feof($fp)) {
    echo fread($fp, 5);
}

--EXPECT--
IS A LOWERCASE STRING.
