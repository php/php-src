--TEST--
PDO::ATTR_ERRMODE
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
$db = MySQLPDOTest::factory();
?>
--INI--
error_reporting=E_ALL
--FILE--
<?php
    require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
    $db = MySQLPDOTest::factory();

    try {
        $db->setAttribute(PDO::ATTR_ERRMODE, []);
    } catch (\Error $e) {
        echo get_class($e), ': ', $e->getMessage(), \PHP_EOL;
    }
    try {
        $db->setAttribute(PDO::ATTR_ERRMODE, new stdClass());
    } catch (\Error $e) {
        echo get_class($e), ': ', $e->getMessage(), \PHP_EOL;
    }
    try {
        /* This currently passes */
        $db->setAttribute(PDO::ATTR_ERRMODE, 'pdo');
    } catch (\Error $e) {
        echo get_class($e), ': ', $e->getMessage(), \PHP_EOL;
    }
    try {
        $db->setAttribute(PDO::ATTR_ERRMODE, 1000);
    } catch (\Error $e) {
        echo get_class($e), ': ', $e->getMessage(), \PHP_EOL;
    }

    $db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
    // no message for any PDO call but...
    $db->query('THIS IS NOT VALID SQL');
    // ... still messages for everything else
    $code = $db->errorCode();
    $info = $db->errorInfo();

    if ($code != '42000')
        printf("[005] Expecting SQL code 42000 got '%s'\n", $code);
    if ($code !== $info[0])
        printf("[006] Code and info should be identical, got errorCode() = %s, errorInfo()[0] = %s\n",
            $code, $info[0]);
    if ('' == $info[1])
        printf("[007] Driver specific error code not set\n");
    if ('' == $info[2])
        printf("[008] Driver specific error message not set\n");

    $db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);
    $db->query('THIS IS NOT VALID SQL');

    $code = $db->errorCode();
    $info = $db->errorInfo();

    if ($code != '42000')
        printf("[009] Expecting SQL code 42000 got '%s'\n", $code);
    if ($code !== $info[0])
        printf("[010] Code and info should be identical, got errorCode() = %s, errorInfo()[0] = %s\n",
            $code, $info[0]);
    if ('' == $info[1])
        printf("[011] Driver specific error code not set\n");
    if ('' == $info[2])
        printf("[012] Driver specific error message not set\n");

    $db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    try {
        $line = __LINE__ + 1;
        $db->query('THIS IS NOT VALID SQL');
    } catch (PDOException $e) {

        $code = $db->errorCode();
        $info = $db->errorInfo();

        if ($code != '42000')
            printf("[013] Expecting SQL code 42000 got '%s'\n", $code);
        if ($code !== $info[0])
            printf("[014] Code and info should be identical, got errorCode() = %s, errorInfo()[0] = %s\n",
                $code, $info[0]);
        if ('' == $info[1])
            printf("[015] Driver specific error code not set\n");
        if ('' == $info[2])
            printf("[016] Driver specific error message not set\n");

        if ($e->getCode() !== $code)
            printf("[017] Exception code '%s' differs from errorCode '%s'\n",
                $e->getCode(), $code);

        $msg = $e->getMessage();
        foreach ($info as $k => $v) {
            if (false === stristr($msg, (string)$v)) {
                printf("[018] Cannot find all parts of the error info ('%s') in the exception message '%s'\n",
                    $v, $msg);
            }
        }

        if ($e->getLine() !== $line)
            printf("[019] Exception has been thrown in line %d, exception object reports line %d\n",
                $line, $e->getLine());

        if ($e->getFile() !== __FILE__)
            printf("[020] Exception has been thrown in file '%s', exception object reports file '%s'\n",
                __FILE__, $e->getFile());

    }

    function my_handler($e) {
        global $db, $line;

        $code = $db->errorCode();
        $info = $db->errorInfo();

        if ($code != '42000')
            printf("[021] Expecting SQL code 42000 got '%s'\n", $code);
        if ($code !== $info[0])
            printf("[022] Code and info should be identical, got errorCode() = %s, errorInfo()[0] = %s\n",
                $code, $info[0]);
        if ('' == $info[1])
            printf("[023] Driver specific error code not set\n");
        if ('' == $info[2])
            printf("[024] Driver specific error message not set\n");

        if ($e->getCode() !== $code)
            printf("[025] Exception code '%s' differs from errorCode '%s'\n",
                $e->getCode(), $code);

        $msg = $e->getMessage();
        foreach ($info as $k => $v) {
            if (false === stristr($msg, (string)$v)) {
                printf("[026] Cannot find all parts of the error info ('%s') in the exception message '%s'\n",
                    $v, $msg);
            }
        }

        if ($e->getLine() !== $line)
            printf("[027] Exception has been thrown in line %d, exception object reports line %d\n",
                $line, $e->getLine());

        if ($e->getFile() !== __FILE__)
            printf("[028] Exception has been thrown in file '%s', exception object reports file '%s'\n",
                __FILE__, $e->getFile());

        if (get_class($e) != 'PDOException')
            printf("[029] Expecting PDO exception got exception of type '%s'\n", get_class($e));

        print "\nend of execution";
    }
    set_exception_handler('my_handler');
    $line = __LINE__ + 1;
    $db->query('THIS IS NOT VALID SQL');

    print "done!\n";
?>
--EXPECTF--
TypeError: Attribute value must be of type int for selected attribute, array given
TypeError: Attribute value must be of type int for selected attribute, stdClass given
ValueError: Error mode must be one of the PDO::ERRMODE_* constants

Warning: PDO::query(): SQLSTATE[42000]: Syntax error or access violation: %d You have an error in your SQL syntax; check the manual that corresponds to your %s server version for the right syntax to use near '%s' at line %d in %s on line %d

end of execution
