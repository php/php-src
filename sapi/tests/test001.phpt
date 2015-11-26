--TEST--
IIS style CGI missing SCRIPT_FILENAME
--DESCRIPTION--
This would be similar to what IIS produces for a simple query.
--ENV--
return <<<END
PATH_TRANSLATED=$filename
PATH_INFO=$scriptname
SCRIPT_NAME=$scriptname
END;
--FILE--
<?php
    echo "HELLO";
?>
--EXPECT--
HELLO