--TEST--
IIS style CGI missing SCRIPT_FILENAME, has PATH_INFO
--DESCRIPTION--
This would be similar to what IIS produces for a simple query
that also has PATH_INFO.
--REQUEST--
return <<<END
PATH_INFO=/path/info
END;
--ENV--
return <<<END
PATH_TRANSLATED=/path/bla
PATH_INFO=/path/info
SCRIPT_NAME=path
END;
--FILE--
<?php
    echo $_SERVER['PATH_INFO'];
?>
--EXPECT--
/path/info