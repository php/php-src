--TEST--
SQLite3::blobOpen test, testing stream with wrong parameter count
--CREDITS--
Michelangelo van Dam
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php
class SQLite3_Test_Stream
{
	private $position;
	public static $string_length = 10;
	public static $string = "abcdefg\0hi";

	public function stream_open($path, $mode, $options, &$opened_path)
	{
		$this->position = 0;
		return true;
	}

	public function stream_read($count)
	{
		$ret = substr(self::$string, $this->position, $count);
		$this->position += strlen($ret);
		return $ret;
	}

	public function stream_write($data)
	{
		return 0;
	}

	public function stream_stat()
	{
		return array('size' => self::$string_length);
	}

	public function stream_tell()
	{
		return $this->position;
	}

	public function stream_eof()
	{
		return ($this->position >= self::$string_length);
	}
}

$db = new SQLite3(':memory:');
stream_wrapper_register('sqliteBlobTest', "SQLite3_Test_Stream") or die("Unable to register sqliteBlobTest stream");
echo "Creating table: " . var_export($db->exec('CREATE TABLE test (id STRING, data BLOB)'),true) . "\n";

echo "PREPARING insert\n";
$insert_stmt = $db->prepare("INSERT INTO test (id, data) VALUES (?, ?)");

echo "BINDING Parameters:\n";
var_dump($insert_stmt->bindValue(1, 'a', SQLITE3_TEXT));
var_dump($insert_stmt->bindValue(2, 'TEST TEST', SQLITE3_BLOB));
$insert_stmt->execute();
echo "Closing statement: " . var_export($insert_stmt->close(), true) . "\n";

echo "Open BLOB with wrong parameter count\n";
$stream = $db->openBlob();
var_dump($stream);
echo "Done\n";
?>
--EXPECTF--
Creating table: true
PREPARING insert
BINDING Parameters:
bool(true)
bool(true)
Closing statement: true
Open BLOB with wrong parameter count

Warning: SQLite3::openBlob() expects at least 3 parameters, 0 given in %s on line %d
NULL
Done
