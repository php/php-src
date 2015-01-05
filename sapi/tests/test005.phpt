--TEST--
QUERY_STRING Security Bug
--DESCRIPTION--
This bug was present in PHP 4.3.0 only.
A failure should print HELLO.
--REQUEST--
return <<<END
SCRIPT_NAME=/nothing.php
QUERY_STRING=$filename
END;
--ENV--
return <<<END
REDIRECT_URL=$scriptname
PATH_TRANSLATED=c:\apache\1.3.27\htdocs\nothing.php
QUERY_STRING=$filename
PATH_INFO=/nothing.php
SCRIPT_NAME=/phpexe/php.exe/nothing.php
SCRIPT_FILENAME=c:\apache\1.3.27\htdocs\nothing.php
END;
--FILE--
<?php
    echo "HELLO";
?>
--EXPECTHEADERS--
Status: 404
--EXPECT--
No input file specified.