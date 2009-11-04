--TEST--
MySQL PDOStatement->fetch(), PDO::FETCH_SERIALIZE
--SKIPIF--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
if (version_compare(PHP_VERSION, '5.1.0', '<'))
	die("skip Needs 5.1.0 and Interface Serializable");
?>
--FILE--
<?php
	require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
	$db = MySQLPDOTest::factory();

	try {

		class myclass implements Serializable {

			private static $instance = null;
			protected $myprotected = 'a protected propery';

			// Good old magic stuff
			private function __construct($caller = NULL) {
				printf("%s(%s)\n", __METHOD__, $caller);
			}


			public function __destruct() {
				// printf("%s()\n", __METHOD__);
			}

			public function __sleep() {
				printf("%s()\n", __METHOD__);
			}

			public function __wakeup() {
				printf("%s()\n", __METHOD__);
			}

			public function __call($method, $params) {
				printf("%s(%s, %s)\n", __METHOD__, $method, var_export($params, true));
			}

			public function __set($prop, $value) {
				printf("%s(%s, %s)\n", __METHOD__, $prop, var_export($value, true));
				$this->{$prop} = $value;
			}

			public function __get($prop) {
				printf("%s(%s)\n", __METHOD__, $prop);
				return NULL;
			}

			// Singleton
			public static function singleton($caller) {
				printf("%s(%s)\n", __METHOD__, $caller);

				if (!self::$instance) {
					$c = __CLASS__;
					self::$instance = new $c($caller);
				}
				return self::$instance;
			}

			// Serializable
			public function serialize() {
				printf("%s()\n", __METHOD__);
				return 'Data from serialize';
			}

			public function unserialize($data) {
				printf("%s(%s)\n", __METHOD__, var_export($data, true));
			}

		}

		$db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 0);
		if (0 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
			printf("[002] Unable to turn off emulated prepared statements\n");

		$db->exec('DROP TABLE IF EXISTS test');
		$db->exec(sprintf('CREATE TABLE test(id INT, myobj BLOB) ENGINE=%s',
			MySQLPDOTest::getTableEngine()));

		printf("Creating an object, serializing it and writing it to DB...\n");
		$id = 1;
		$obj = myclass::singleton('Creating object');
		$myobj = serialize($obj);
		$stmt = $db->prepare('INSERT INTO test(id, myobj) VALUES (?, ?)');
		$stmt->bindValue(1, $id);
		$stmt->bindValue(2, $myobj);
		$stmt->execute();

		printf("\nUnserializing the previously serialized object...\n");
		var_dump(unserialize($myobj));

		printf("\nUsing PDO::FETCH_CLASS|PDO::FETCH_SERIALIZE to fetch the object from DB and unserialize it...\n");
		$stmt = $db->prepare('SELECT myobj FROM test');
		$stmt->setFetchMode(PDO::FETCH_CLASS|PDO::FETCH_SERIALIZE, 'myclass', array('PDO shall not call __construct()'));
		$stmt->execute();
		var_dump($stmt->fetch());

		printf("\nUsing PDO::FETCH_CLASS to fetch the object from DB and unserialize it...\n");
		$stmt = $db->prepare('SELECT myobj FROM test');
		$stmt->setFetchMode(PDO::FETCH_CLASS, 'myclass', array('PDO shall call __construct()'));
		$stmt->execute();
		var_dump($stmt->fetch());


	} catch (PDOException $e) {
		printf("[001] %s [%s] %s\n",
			$e->getMessage(), $db->errorCode(), implode(' ', $db->errorInfo()));
	}

	print "done!\n";
?>
--CLEAN--
<?php
require dirname(__FILE__) . '/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS test');
?>
--EXPECTF--
Creating an object, serializing it and writing it to DB...
myclass::singleton(Creating object)
myclass::__construct(Creating object)
myclass::serialize()

Unserializing the previously serialized object...
myclass::unserialize('Data from serialize')
object(myclass)#4 (1) {
  [%u|b%"myprotected":protected]=>
  %unicode|string%(19) "a protected propery"
}

Using PDO::FETCH_CLASS|PDO::FETCH_SERIALIZE to fetch the object from DB and unserialize it...
myclass::unserialize('C:7:"myclass":19:{Data from serialize}')
object(myclass)#%d (1) {
  [%u|b%"myprotected":protected]=>
  %unicode|string%(19) "a protected propery"
}

Using PDO::FETCH_CLASS to fetch the object from DB and unserialize it...
myclass::__set(myobj, 'C:7:"myclass":19:{Data from serialize}')
myclass::__construct(PDO shall call __construct())
object(myclass)#%d (2) {
  [%u|b%"myprotected":protected]=>
  %unicode|string%(19) "a protected propery"
  [%u|b%"myobj"]=>
  %unicode|string%(38) "C:7:"myclass":19:{Data from serialize}"
}
done!
