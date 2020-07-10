--TEST--
mysqli_fetch_object()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require_once("connect.inc");
    set_error_handler('handle_catchable_fatal');

    $mysqli = new mysqli();
    try {
        new mysqli_result($mysqli);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    require('table.inc');
    if (!$mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket))
        printf("[002] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);

    if (!$res = $mysqli->query("SELECT id AS ID, label FROM test AS TEST ORDER BY id LIMIT 5")) {
        printf("[003] [%d] %s\n", $mysqli->errno, $mysqli->error);
    }

    try {
        if (!is_null($tmp = @$res->fetch_object($link, $link)))
            printf("[005] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);
    } catch (Error $e) {
        handle_catchable_fatal($e->getCode(), $e->getMessage(), $e->getFile(), $e->getLine());
    }


    try {
        if (!is_null($tmp = @$res->fetch_object($link, $link, $link)))
            printf("[006] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);
    } catch (Error $e) {
        handle_catchable_fatal($e->getCode(), $e->getMessage(), $e->getFile(), $e->getLine());
    }

    $obj = mysqli_fetch_object($res);
    if (($obj->ID !== "1") || ($obj->label !== "a") || (get_class($obj) != 'stdClass')) {
        printf("[007] Object seems wrong. [%d] %s\n", $mysqli->errno, $mysqli->error);
        var_dump($obj);
    }

    class mysqli_fetch_object_test {

        public $a = null;
        public $b = null;

        public function toString() {
            var_dump($this);
        }
    }

    $obj = $res->fetch_object('mysqli_fetch_object_test');
    if (($obj->ID !== "2") || ($obj->label !== "b") || ($obj->a !== NULL) || ($obj->b !== NULL) || (get_class($obj) != 'mysqli_fetch_object_test')) {
        printf("[008] Object seems wrong. [%d] %s\n", $mysqli->errno, $mysqli->error);
        var_dump($obj);
    }

    class mysqli_fetch_object_construct extends mysqli_fetch_object_test {

        public function __construct($a, $b) {
            $this->a = $a;
            $this->b = $b;
        }

    }

    try {
        $obj = $res->fetch_object('mysqli_fetch_object_construct', null);

        if (($obj->ID !== "3") || ($obj->label !== "c") || ($obj->a !== NULL) || ($obj->b !== NULL) || (get_class($obj) != 'mysqli_fetch_object_construct')) {
            printf("[009] Object seems wrong. [%d] %s\n", $mysqli->errno, $mysqli->error);
            var_dump($obj);
        }
    } catch (Error $e) {
        handle_catchable_fatal($e->getCode(), $e->getMessage(), $e->getFile(), $e->getLine());
        mysqli_fetch_object($res);
    }

    try {
        $obj = $res->fetch_object('mysqli_fetch_object_construct', array('a'));
        if (($obj->ID !== "4") || ($obj->label !== "d") || ($obj->a !== 'a') || ($obj->b !== NULL) || (get_class($obj) != 'mysqli_fetch_object_construct')) {
            printf("[010] Object seems wrong. [%d] %s\n", $mysqli->errno, $mysqli->error);
            var_dump($obj);
        }
    } catch (Throwable $e) {
        echo "Exception: " . $e->getMessage() . "\n";
    }

    $obj = $res->fetch_object('mysqli_fetch_object_construct', array('a', 'b'));
    if (($obj->ID !== "5") || ($obj->label !== "e") || ($obj->a !== 'a') || ($obj->b !== 'b') || (get_class($obj) != 'mysqli_fetch_object_construct')) {
        printf("[011] Object seems wrong. [%d] %s\n", $mysqli->errno, $mysqli->error);
        var_dump($obj);
    }

    var_dump($res->fetch_object('mysqli_fetch_object_construct', array('a', 'b', 'c')));
    var_dump(mysqli_fetch_object($res));

    mysqli_free_result($res);

    if (!$res = mysqli_query($link, "SELECT id AS ID, label FROM test AS TEST")) {
        printf("[012] [%d] %s\n", $mysqli->errno, $mysqli->error);
    }

    mysqli_free_result($res);

    try {
        mysqli_fetch_object($res);
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }

    // Fatal error, script execution will end
    var_dump($res->fetch_object('this_class_does_not_exist'));

    $mysqli->close();
    print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
mysqli object is not fully initialized
[0] mysqli_result::fetch_object(): Argument #1 ($class_name) must be of type string, mysqli given in %s on line %d
[0] mysqli_result::fetch_object() expects at most 2 parameters, 3 given in %s on line %d
[0] mysqli_result::fetch_object(): Argument #2 ($params) must be of type array, null given in %s on line %d
Exception: Too few arguments to function mysqli_fetch_object_construct::__construct(), 1 passed and exactly 2 expected
NULL
NULL
mysqli_result object is already closed

Fatal error: Class "this_class_does_not_exist" not found in %s on line %d
