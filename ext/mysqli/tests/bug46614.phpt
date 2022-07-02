--TEST--
Bug #46614 (Extended MySQLi class gives incorrect empty() result)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
if (!defined("MYSQLI_ASYNC")) {
    die("skip mysqlnd only");
}
?>
--FILE--
<?php
class MySQL_Ext extends mysqli{
    protected $fooData = array();
    private $extData;

    public function isEmpty()
    {
        $this->extData[] = 'Bar';
        return empty($this->extData);
    }
}

include ("connect.inc");
$MySQL_Ext = new MySQL_Ext($host, $user, $passwd, $db, $port, $socket);

$isEmpty = $MySQL_Ext->isEmpty();
var_dump($isEmpty);
?>
--EXPECT--
bool(false)
