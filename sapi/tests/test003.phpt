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
PATH_TRANSLATED=$filename/path/info
PATH_INFO=$scriptname/path/info
SCRIPT_NAME=$scriptname
END;
--FILE--
<?php
    echo $_SERVER['PATH_INFO'];
?>
--EXPECT--
/path/info