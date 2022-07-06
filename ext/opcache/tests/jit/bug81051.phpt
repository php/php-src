--TEST--
Bug #80839: PHP problem with JIT
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=1M
opcache.jit=1205
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
class Binary{
	public static function readUnsignedVarInt(string $buffer, int &$offset) : int{
		$offset++;
		return 0;
	}
}

class BinaryStream{

	private string $buffer;
	private int $offset;

	public function __construct(string $buffer, int $offset = 0){
		$this->buffer = $buffer;
		$this->offset = $offset;
	}

	public function getUnsignedVarInt() : int{
		return Binary::readUnsignedVarInt($this->buffer, $this->offset);
	}

	public function get(int $len) : string{
		return $len === 1 ? $this->buffer[$this->offset++] : substr($this->buffer, ($this->offset += $len) - $len, $len);
	}
}
$stream = new BinaryStream(str_repeat("\x01a", 1000));
var_dump($stream->getUnsignedVarInt());
var_dump($stream->get(1));
?>
--EXPECT--
int(0)
string(1) "a"
