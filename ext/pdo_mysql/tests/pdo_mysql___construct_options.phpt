--TEST--
MySQL PDO->__construct(), options
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
    require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

    function set_option_and_check($offset, $option, $value, $option_desc) {

        $dsn = MySQLPDOTest::getDSN();
        $user = PDO_MYSQL_TEST_USER;
        $pass = PDO_MYSQL_TEST_PASS;

        try {
            $db = new PDO($dsn, $user, $pass, array($option => $value));
            $db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
            if (!is_object($db) || ($value !== ($tmp = @$db->getAttribute($option))))
                printf("[%03d] Expecting '%s'/%s got '%s'/%s' for options '%s'\n",
                    $offset,
                    $value, gettype($value),
                    $tmp, gettype($tmp),
                    $option_desc);
        } catch (PDOException $e) {
            printf("[%03d] %s\n", $offset, $e->getMessage());
        }

    }

    try {

        $dsn = MySQLPDOTest::getDSN();
        $user = PDO_MYSQL_TEST_USER;
        $pass = PDO_MYSQL_TEST_PASS;

        $valid_options = array(
            /* pdo_dbh.c */
            PDO::ATTR_PERSISTENT									=> 'PDO::ATTR_PERSISTENT',
            PDO::ATTR_AUTOCOMMIT									=> 'PDO::ATTR_AUTOCOMMIT',
            /* mysql_driver.c */
            /* TODO Possible bug PDO::ATTR_TIMEOUT != MYSQLI_OPT_CONNECT_TIMEOUT*/
            PDO::ATTR_TIMEOUT 										=> 'PDO::ATTR_TIMEOUT',
            PDO::ATTR_EMULATE_PREPARES						=> 'PDO::ATTR_EMULATE_PREPARES',

            PDO::MYSQL_ATTR_USE_BUFFERED_QUERY		=> 'PDO::MYSQL_ATTR_USE_BUFFERED_QUERY',
            PDO::MYSQL_ATTR_LOCAL_INFILE					=> 'PDO::MYSQL_ATTR_LOCAL_INFILE',
            PDO::MYSQL_ATTR_DIRECT_QUERY					=> 'PDO::MYSQL_ATTR_DIRECT_QUERY',

            PDO::MYSQL_ATTR_INIT_COMMAND					=> 'PDO::MYSQL_ATTR_INIT_COMMAND',
            PDO::ATTR_EMULATE_PREPARES						=> 'PDO::ATTR_EMULATE_PREPARES',
        );

        $defaults = array(
            PDO::ATTR_PERSISTENT									=> false,
            PDO::ATTR_AUTOCOMMIT									=> 1,
            /* TODO - why is this a valid option if getAttribute() does not support it?! */
            PDO::ATTR_TIMEOUT 										=> false,
            PDO::ATTR_EMULATE_PREPARES						=> 1,
            PDO::MYSQL_ATTR_USE_BUFFERED_QUERY		=> true,
            /* TODO getAttribute() does not handle it */
            PDO::MYSQL_ATTR_LOCAL_INFILE					=> false,
            /* TODO getAttribute() does not handle it */
            PDO::MYSQL_ATTR_DIRECT_QUERY					=> 1,
            PDO::MYSQL_ATTR_INIT_COMMAND					=> '',
        );

        try {
            if (NULL !== ($db = @new PDO($dsn, $user, $pass, 'wrong type')))
                printf("[001] Expecting NULL got %s/%s\n", gettype($db), $db);
        } catch (TypeError $e) {
        }

        if (!is_object($db = new PDO($dsn, $user, $pass, array())))
            printf("[002] Expecting object got %s/%s¸\n", gettype($db), $db);

        $invalid = 999;
        if (is_object($db = new PDO($dsn, $user, $pass, array($invalid => true))))
            printf("[003] [TODO][CHANGEREQUEST] Please, lets not ignore invalid options and bail out!\n");

        $db = new PDO($dsn, $user, $pass);
        $db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
        foreach ($valid_options as $option => $name) {
            /* TODO getAttribute() is pretty poor in supporting the options, suppress errors */
            $tmp = @$db->getAttribute($option);
            if ($tmp !== $defaults[$option])
                printf("[003a] Expecting default value for '%s' of '%s'/%s, getAttribute() reports setting '%s'/%s\n",
                    $name, $defaults[$option], gettype($defaults[$option]),
                    $tmp, gettype($tmp));
        }

        $db = new PDO($dsn, $user, $pass, array(PDO::ATTR_AUTOCOMMIT => true));
        if (!is_object($db) || !$db->getAttribute(PDO::ATTR_AUTOCOMMIT))
            printf("[004] Autocommit should be on\n");

        $db = new PDO($dsn, $user, $pass, array(PDO::ATTR_AUTOCOMMIT => false));
        if (!is_object($db) || $db->getAttribute(PDO::ATTR_AUTOCOMMIT))
            printf("[005] Autocommit should be off\n");

        /* TODO: no way to check ATTR_TIMEOUT settings */
        if (!is_object($db = new PDO($dsn, $user, $pass, array(PDO::ATTR_TIMEOUT => 10))))
            printf("[006] ATTR_TIMEOUT should be accepted\n");

        if (!is_object($db = new PDO($dsn, $user, $pass, array(PDO::ATTR_TIMEOUT => PHP_INT_MAX))))
            printf("[007] ATTR_TIMEOUT should be accepted\n");

        if (!is_object($db = new PDO($dsn, $user, $pass, array(PDO::ATTR_TIMEOUT => -PHP_INT_MAX))))
            printf("[008] ATTR_TIMEOUT should be accepted\n");

        /* TODO: Its ugly that PDO::ATTR_EMULATE_PREPARES == PDO::MYSQL_ATTR_DIRECT_QUERY */
        $db = new PDO($dsn, $user, $pass, array(PDO::ATTR_EMULATE_PREPARES => true));
        if (!is_object($db))
            printf("[009] ATTR_EMULATE_PREPARES should be accepted and on\n");
        if (!$db->getAttribute(PDO::ATTR_EMULATE_PREPARES))
            printf("[010] [TODO][CHANGEREQUEST] ATTR_EMULATE_PREPARES should be on\n");
        if (!$db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
            printf("[011] As PDO::MYSQL_ATTR_DIRECT_QUERY == PDO::ATTR_EMULATE_PREPARES
                and PDO::ATTR_EMULATE_PREPARES overrules the other, PDO::MYSQL_ATTR_DIRECT_QUERY should be on\n");

        $db = new PDO($dsn, $user, $pass, array(PDO::ATTR_EMULATE_PREPARES => false));
        if (!is_object($db))
            printf("[012] ATTR_EMULATE_PREPARES should be accepted and on\n");
        if ($db->getAttribute(PDO::ATTR_EMULATE_PREPARES))
            printf("[013] [TODO][CHANGEREQUEST] ATTR_EMULATE_PREPARES should be off\n");
        if ($db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
            printf("[014] As PDO::MYSQL_ATTR_DIRECT_QUERY == PDO::ATTR_EMULATE_PREPARES
                and PDO::ATTR_EMULATE_PREPARES overrules the other, PDO::MYSQL_ATTR_DIRECT_QUERY should be off\n");

        // PDO::ATTR_EMULATE_PREPARES overrules PDO::MYSQL_ATTR_DIRECT_QUERY
        // TODO: is it clever that a generic setting overrules a specific setting?
        $db = new PDO($dsn, $user, $pass, array(PDO::ATTR_EMULATE_PREPARES => true, PDO::MYSQL_ATTR_DIRECT_QUERY => false));
        if (!$db->getAttribute(PDO::ATTR_EMULATE_PREPARES))
            printf("[015] PDO::ATTR_EMULATE_PREPARES should be on\n");
        if (!$db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
            printf("[016] PDO::MYSQL_ATTR_DIRECT_QUERY should be on\n");

        $db = new PDO($dsn, $user, $pass, array(PDO::ATTR_EMULATE_PREPARES => false, PDO::MYSQL_ATTR_DIRECT_QUERY => true));
        if ($db->getAttribute(PDO::ATTR_EMULATE_PREPARES))
            printf("[017] PDO::ATTR_EMULATE_PREPARES should be off\n");
        if ($db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
            printf("[018] PDO::MYSQL_ATTR_DIRECT_QUERY should be off\n");

        set_option_and_check(19, PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, true, 'PDO::MYSQL_ATTR_USE_BUFFERED_QUERY');
        set_option_and_check(20, PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, false, 'PDO::MYSQL_ATTR_USE_BUFFERED_QUERY');

        set_option_and_check(21, PDO::MYSQL_ATTR_LOCAL_INFILE, true, 'PDO::MYSQL_ATTR_LOCAL_INFILE');
        set_option_and_check(22, PDO::MYSQL_ATTR_LOCAL_INFILE, false, 'PDO::MYSQL_ATTR_LOCAL_INFILE');

        set_option_and_check(23, PDO::MYSQL_ATTR_INIT_COMMAND, 'SET @a=1', 'PDO::MYSQL_ATTR_INIT_COMMAND');
        set_option_and_check(24, PDO::MYSQL_ATTR_INIT_COMMAND, '', 'PDO::MYSQL_ATTR_INIT_COMMAND');
        set_option_and_check(25, PDO::MYSQL_ATTR_INIT_COMMAND, 'INSERT INTO nonexistent(invalid) VALUES (1)', 'PDO::MYSQL_ATTR_INIT_COMMAND');

        set_option_and_check(33, PDO::MYSQL_ATTR_DIRECT_QUERY, 1, 'PDO::MYSQL_ATTR_DIRECT_QUERY');
        set_option_and_check(34, PDO::MYSQL_ATTR_DIRECT_QUERY, 0, 'PDO::MYSQL_ATTR_DIRECT_QUERY');

        if (defined('PDO::MYSQL_ATTR_LOCAL_INFILE_DIRECTORY')) {
            set_option_and_check(35, PDO::MYSQL_ATTR_LOCAL_INFILE_DIRECTORY, null, 'PDO::MYSQL_ATTR_LOCAL_INFILE_DIRECTORY');
            // libmysqlclient returns the directory with a trailing slash.
            // set_option_and_check(36, PDO::MYSQL_ATTR_LOCAL_INFILE_DIRECTORY, __DIR__, 'PDO::MYSQL_ATTR_LOCAL_INFILE_DIRECTORY');
        }
    } catch (PDOException $e) {
        printf("[001] %s, [%s] %s Line: %s\n",
            $e->getMessage(),
            (is_object($db)) ? $db->errorCode() : 'n/a',
            (is_object($db)) ? implode(' ', $db->errorInfo()) : 'n/a',
            $e->getLine());
    }

    print "done!";
?>
--EXPECTF--
[003] [TODO][CHANGEREQUEST] Please, lets not ignore invalid options and bail out!
[003a] Expecting default value for 'PDO::MYSQL_ATTR_INIT_COMMAND' of ''/string, getAttribute() reports setting ''/boolean
[015] PDO::ATTR_EMULATE_PREPARES should be on
[016] PDO::MYSQL_ATTR_DIRECT_QUERY should be on
[017] PDO::ATTR_EMULATE_PREPARES should be off
[018] PDO::MYSQL_ATTR_DIRECT_QUERY should be off
[023] Expecting 'SET @a=1'/string got ''/boolean' for options 'PDO::MYSQL_ATTR_INIT_COMMAND'
[024] SQLSTATE[42000] [1065] Query was empty
[025] SQLSTATE[42S02] [1146] Table '%s.nonexistent' doesn't exist
done!
