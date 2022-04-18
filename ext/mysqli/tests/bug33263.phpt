--TEST--
Bug #33263 (mysqli_real_connect in __construct)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");

    class test extends mysqli
    {
        public function __construct($host, $user, $passwd, $db, $port, $socket) {
            parent::__construct();
            parent::real_connect($host, $user, $passwd, $db, $port, $socket);
        }
    }

    $mysql = new test($host, $user, $passwd, $db, $port, $socket);

    $stmt = $mysql->prepare("SELECT DATABASE()");
    $stmt->execute();
    $stmt->bind_result($database);
    $stmt->fetch();
    $stmt->close();

    if ($database != $db)
        printf("[001] Expecting '%s' got %s/'%s'.\n",
            gettype($database), $database);

    $mysql->close();
    print "done!";
?>
--EXPECT--
done!
