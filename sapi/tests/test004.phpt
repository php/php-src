--TEST--
Apache style CGI with PATH_INFO
--DESCRIPTION--
Apache likes to set SCRIPT_FILENAME to the php executable
if you use ScriptAlias configurations, and the proper
path is in PATH_TRANSLATED.  SCRIPT_NAME in this is faked,
but that is ok, Apache sets SCRIPT_NAME to the ScriptAlias
of the executable.
--REQUEST--
return <<<END
PATH_INFO=/path/info
END;
--ENV--
return <<<END
REDIRECT_URL=/path
PATH_TRANSLATED=/path/info/fpp
PATH_INFO=/path/info
SCRIPT_NAME=/scriptalias/php
SCRIPT_FILENAME=$this->conf['TEST_PHP_EXECUTABLE']
END;
--FILE--
<?php
    echo $_SERVER['PATH_INFO'];
?>
--EXPECT--
/path/info
