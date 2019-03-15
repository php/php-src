--TEST--
Bug #63176 (Segmentation fault when instantiate 2 persistent PDO to the same db server)
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
require(__DIR__. DIRECTORY_SEPARATOR . 'config.inc');
class PDO2 extends PDO {
	protected $transLevel;
}

class PDO3 extends PDO {
	protected $tomato;
}


class ModelA {
	public function __construct($h) {
		var_dump($h);
		if ($h) {
			$this->db = new PDO2(PDO_MYSQL_TEST_DSN, PDO_MYSQL_TEST_USER, PDO_MYSQL_TEST_PASS, array(PDO::ATTR_PERSISTENT => true));
		} else {
			$this->db = new PDO3(PDO_MYSQL_TEST_DSN, PDO_MYSQL_TEST_USER, PDO_MYSQL_TEST_PASS, array(PDO::ATTR_PERSISTENT => true));
		}
		$this->db->query('SELECT 1')->fetchAll();
	}
}

$a = new ModelA(true);
$b = new ModelA(false);

var_dump($a);
var_dump($b);
--EXPECTF--
bool(true)
bool(false)
object(ModelA)#%d (1) {
  ["db"]=>
  object(PDO2)#%d (1) {
    ["transLevel":protected]=>
    NULL
  }
}
object(ModelA)#%d (1) {
  ["db"]=>
  object(PDO3)#%d (1) {
    ["tomato":protected]=>
    NULL
  }
}
