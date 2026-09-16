--TEST--
UserCache\Cache: shaped associative state arrays preserve unserialize semantics
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
class ShapedArrayStatePayload
{
	public static array $events = [];
	public int $id = 0;
	public string $label = '';
	public int $checksum = 0;

	public function __construct(int $id, string $label)
	{
		$this->id = $id;
		$this->label = $label;
	}

	public function __serialize(): array
	{
		return [
			'id' => $this->id,
			'label' => $this->label,
			'nested' => [
				'id' => $this->id,
				'label' => $this->label,
				'checksum' => $this->id + strlen($this->label),
			],
		];
	}

	public function __unserialize(array $data): void
	{
		$GLOBALS['shaped_array_state_unserialize_count']++;
		self::$events[] = array_keys($data);
		$this->id = $data['id'];
		$this->label = $data['label'];
		$this->checksum = $data['nested']['checksum'];
	}
}

$GLOBALS['shaped_array_state_unserialize_count'] = 0;

$cache = UserCache\Cache::getPool('shaped-array-state');
$payload = [
	new ShapedArrayStatePayload(10, 'first'),
	new ShapedArrayStatePayload(20, 'second'),
	new ShapedArrayStatePayload(30, 'third'),
];

var_dump($cache->store('payload', $payload));

$first = $cache->fetch('payload');
$second = $cache->fetch('payload');

echo $GLOBALS['shaped_array_state_unserialize_count'], "\n";
echo $first[0]->checksum, ",", $first[1]->checksum, ",", $first[2]->checksum, "\n";
echo $second[0]->checksum, ",", $second[1]->checksum, ",", $second[2]->checksum, "\n";
var_dump(ShapedArrayStatePayload::$events);
?>
--EXPECT--
bool(true)
6
15,26,35
15,26,35
array(6) {
  [0]=>
  array(3) {
    [0]=>
    string(2) "id"
    [1]=>
    string(5) "label"
    [2]=>
    string(6) "nested"
  }
  [1]=>
  array(3) {
    [0]=>
    string(2) "id"
    [1]=>
    string(5) "label"
    [2]=>
    string(6) "nested"
  }
  [2]=>
  array(3) {
    [0]=>
    string(2) "id"
    [1]=>
    string(5) "label"
    [2]=>
    string(6) "nested"
  }
  [3]=>
  array(3) {
    [0]=>
    string(2) "id"
    [1]=>
    string(5) "label"
    [2]=>
    string(6) "nested"
  }
  [4]=>
  array(3) {
    [0]=>
    string(2) "id"
    [1]=>
    string(5) "label"
    [2]=>
    string(6) "nested"
  }
  [5]=>
  array(3) {
    [0]=>
    string(2) "id"
    [1]=>
    string(5) "label"
    [2]=>
    string(6) "nested"
  }
}
