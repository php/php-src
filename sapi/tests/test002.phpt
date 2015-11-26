--TEST--
Apache style CGI
--DESCRIPTION--
Apache likes to set SCRIPT_FILENAME to the php executable
if you use ScriptAlias configurations, and the proper
path is in PATH_TRANSLATED.  SCRIPT_NAME in this is faked,
but that is ok, Apache sets SCRIPT_NAME to the ScriptAlias
of the executable.
--ENV--
return <<<END
REDIRECT_URL=$scriptname
PATH_TRANSLATED=$filename
PATH_INFO=$scriptname
SCRIPT_NAME=/scriptalias/php
SCRIPT_FILENAME=$this->conf['TEST_PHP_EXECUTABLE']
END;
--FILE--
<?php
    echo "HELLO";
?>
--EXPECT--
HELLO