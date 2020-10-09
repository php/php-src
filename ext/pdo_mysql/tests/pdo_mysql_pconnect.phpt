--TEST--
MySQL PDO->__construct(), PDO::ATTR_PERSISTENT
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
    require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

    try {

        $dsn = MySQLPDOTest::getDSN();
        $user = PDO_MYSQL_TEST_USER;
        $pass = PDO_MYSQL_TEST_PASS;

        $db1 = new PDO($dsn, $user, $pass, array(PDO::ATTR_PERSISTENT => true));
        $db2 = new PDO($dsn, $user, $pass, array(PDO::ATTR_PERSISTENT => true));
        $db1->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
        $db2->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
        $db1->exec('SET @pdo_persistent_connection=1');
        $stmt = $db2->query('SELECT @pdo_persistent_connection as _pers');
        $tmp = $stmt->fetch(PDO::FETCH_ASSOC);
        if ($tmp['_pers'] !== '1')
            printf("[001] Both handles should use the same connection.");

        $stmt = $db1->query('SELECT CONNECTION_ID() as _con1');
        $tmp = $stmt->fetch(PDO::FETCH_ASSOC);
        $con1 = $tmp['_con1'];

        $stmt = $db2->query('SELECT CONNECTION_ID() as _con2');
        $tmp = $stmt->fetch(PDO::FETCH_ASSOC);
        $con2 = $tmp['_con2'];

        if ($con1 !== $con2)
            printf("[002] Both handles should report the same MySQL thread ID");

        $db1 = NULL; /* should be equal to closing to my understanding */
        $db1 = new PDO($dsn, $user, $pass, array(PDO::ATTR_PERSISTENT => true));
        $db1->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
        $stmt = $db1->query('SELECT CONNECTION_ID() as _con1');
        $tmp = $stmt->fetch(PDO::FETCH_ASSOC);
        $con1 = $tmp['_con1'];

        if ($con1 !== $con2)
            printf("[003] Both handles should report the same MySQL thread ID");

        $affected = $db1->exec(sprintf('KILL %d', $con1));
        // Server needs some think-time sometimes
        sleep(1);
        if ('00000' == $db1->errorCode()) {
            // looks like KILL has worked ? Or not... TODO: why no warning with libmysql?!
            @$db1->exec("SET @pdo_persistent_connection=2");
            // but now I want to see some error...
            if ('HY000' != $db1->errorCode())
                printf("[004] Wrong error code %s\n", $db1->errorCode());

            $tmp = implode(' ', $db1->errorInfo());
            if (!strstr($tmp, '2006'))
                printf("[005] Wrong error info %s\n", $tmp);
        }

        $db1 = new PDO($dsn, $user, $pass, array(PDO::ATTR_PERSISTENT => false));
        $db1->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
        $stmt = $db1->query('SELECT CONNECTION_ID() as _con1');
        $tmp = $stmt->fetch(PDO::FETCH_ASSOC);
        $con1 = $tmp['_con1'];

        @$db2 = new PDO($dsn, $user, $pass, array(PDO::ATTR_PERSISTENT => true));
        $db2->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
        $stmt = $db2->query('SELECT CONNECTION_ID() as _con2');
        $tmp = $stmt->fetch(PDO::FETCH_ASSOC);
        $con2 = $tmp['_con2'];

        if ($con1 == $con2)
            printf("[006] Looks like the persistent and the non persistent connection are using the same link?!\n");

        // lets go crazy and create a few pconnections...
        $connections = array();
        for ($i = 0; $i <= 20; $i++) {
            $connections[$i] = new PDO($dsn, $user, $pass, array(PDO::ATTR_PERSISTENT => true));
        }
        do {
            $i = mt_rand(0, 20);
            if (isset($connections[$i]))
                unset($connections[$i]);
        } while (!empty($connections));


    } catch (PDOException $e) {
        printf("[001] %s, [%s] %s [%s] %s\n",
            $e->getMessage(),
            (is_object($db1)) ? $db1->errorCode() : 'n/a',
            (is_object($db1)) ? implode(' ', $db1->errorInfo()) : 'n/a',
            (is_object($db2)) ? $db2->errorCode() : 'n/a',
            (is_object($db2)) ? implode(' ', $db2->errorInfo()) : 'n/a');
    }

    print "done!";
?>
--EXPECT--
done!
