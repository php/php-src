--TEST--
Bug #32001 (xml_parse*() goes into infinite loop when autodetection in effect), using EUC-JP, Shift_JIS, GB2312
--SKIPIF--
<?php
require_once("skipif.inc");
if (!extension_loaded('iconv')) die ("skip iconv extension not available");
foreach(array('EUC-JP', 'Shift_JISP', 'GB2312') as $encoding) {
	if (@xml_parser_create($encoding) === false) die("skip libxml2 does not support $encoding encoding");
}
?>
--FILE--
<?php
class testcase {
	private $encoding;
	private $bom;
	private $prologue;
	private $tags;
	private $chunk_size;

	function testcase($enc, $chunk_size = 0, $bom = 0, $omit_prologue = 0) {
		$this->encoding = $enc;
		$this->chunk_size = $chunk_size;
		$this->bom = $bom;
		$this->prologue = !$omit_prologue;
		$this->tags = array();
	}

	function start_element($parser, $name, $attrs) {
		$attrs = array_map('bin2hex', $attrs);
		$this->tags[] = bin2hex($name).": ".implode(', ', $attrs);
	}

	function end_element($parser, $name) {
	}

	function run() {
		$data = '';

		if ($this->prologue) {
			$canonical_name = preg_replace('/BE|LE/i', '', $this->encoding);
			$data .= "<?xml version=\"1.0\" encoding=\"$canonical_name\" ?>\n";
		}

		$data .= <<<HERE
<テスト:テスト1 xmlns:テスト="http://www.example.com/テスト/" テスト="テスト">
  <テスト:テスト2 テスト="テスト">
	<テスト:テスト3>
	  test! 
	</テスト:テスト3>
  </テスト:テスト2>
</テスト:テスト1>
HERE;

		$data = iconv("UTF-8", $this->encoding, $data);

		$parser = xml_parser_create(NULL);
		xml_parser_set_option($parser, XML_OPTION_CASE_FOLDING, 0);
		xml_set_element_handler($parser, "start_element", "end_element");
		xml_set_object($parser, $this);

		if ($this->chunk_size == 0) {
			$success = @xml_parse($parser, $data, true);
		} else {
			for ($offset = 0; $offset < strlen($data);
					$offset += $this->chunk_size) {
				$success = @xml_parse($parser, substr($data, $offset, $this->chunk_size), false);
				if (!$success) {
					break;
				}
			}
			if ($success) {
				$success = @xml_parse($parser, "", true);
			}
		}

		echo "Encoding: $this->encoding\n";
		echo "XML Prologue: ".($this->prologue ? 'present': 'not present'), "\n";
		echo "Chunk size: ".($this->chunk_size ? "$this->chunk_size byte(s)\n": "all data at once\n");
		echo "BOM: ".($this->bom ? 'prepended': 'not prepended'), "\n";

		if ($success) {
			var_dump($this->tags);
		} else {
			echo "[Error] ", xml_error_string(xml_get_error_code($parser)), "\n";
		}
	}
}
$suite = array(
	new testcase("EUC-JP"  ,  0),
	new testcase("EUC-JP"  ,  1),
	new testcase("Shift_JIS", 0),
	new testcase("Shift_JIS", 1),
	new testcase("GB2312",    0),
	new testcase("GB2312",    1),
);

if (XML_SAX_IMPL == 'libxml') {
  echo "libxml2 Version => " . LIBXML_DOTTED_VERSION. "\n";
} else {
  echo "libxml2 Version => NONE\n";
}

foreach ($suite as $testcase) {
	$testcase->run();
}

?>
--EXPECTF--
libxml2 Version => %s
Encoding: EUC-JP
XML Prologue: present
Chunk size: all data at once
BOM: not prepended
array(3) {
  [0]=>
  string(128) "e38386e382b9e383883ae38386e382b9e3838831: 687474703a2f2f7777772e6578616d706c652e636f6d2fe38386e382b9e383882f, e38386e382b9e38388"
  [1]=>
  string(60) "e38386e382b9e383883ae38386e382b9e3838832: e38386e382b9e38388"
  [2]=>
  string(42) "e38386e382b9e383883ae38386e382b9e3838833: "
}
Encoding: EUC-JP
XML Prologue: present
Chunk size: 1 byte(s)
BOM: not prepended
array(3) {
  [0]=>
  string(128) "e38386e382b9e383883ae38386e382b9e3838831: 687474703a2f2f7777772e6578616d706c652e636f6d2fe38386e382b9e383882f, e38386e382b9e38388"
  [1]=>
  string(60) "e38386e382b9e383883ae38386e382b9e3838832: e38386e382b9e38388"
  [2]=>
  string(42) "e38386e382b9e383883ae38386e382b9e3838833: "
}
Encoding: Shift_JIS
XML Prologue: present
Chunk size: all data at once
BOM: not prepended
array(3) {
  [0]=>
  string(128) "e38386e382b9e383883ae38386e382b9e3838831: 687474703a2f2f7777772e6578616d706c652e636f6d2fe38386e382b9e383882f, e38386e382b9e38388"
  [1]=>
  string(60) "e38386e382b9e383883ae38386e382b9e3838832: e38386e382b9e38388"
  [2]=>
  string(42) "e38386e382b9e383883ae38386e382b9e3838833: "
}
Encoding: Shift_JIS
XML Prologue: present
Chunk size: 1 byte(s)
BOM: not prepended
array(3) {
  [0]=>
  string(128) "e38386e382b9e383883ae38386e382b9e3838831: 687474703a2f2f7777772e6578616d706c652e636f6d2fe38386e382b9e383882f, e38386e382b9e38388"
  [1]=>
  string(60) "e38386e382b9e383883ae38386e382b9e3838832: e38386e382b9e38388"
  [2]=>
  string(42) "e38386e382b9e383883ae38386e382b9e3838833: "
}
Encoding: GB2312
XML Prologue: present
Chunk size: all data at once
BOM: not prepended
array(3) {
  [0]=>
  string(128) "e38386e382b9e383883ae38386e382b9e3838831: 687474703a2f2f7777772e6578616d706c652e636f6d2fe38386e382b9e383882f, e38386e382b9e38388"
  [1]=>
  string(60) "e38386e382b9e383883ae38386e382b9e3838832: e38386e382b9e38388"
  [2]=>
  string(42) "e38386e382b9e383883ae38386e382b9e3838833: "
}
Encoding: GB2312
XML Prologue: present
Chunk size: 1 byte(s)
BOM: not prepended
array(3) {
  [0]=>
  string(128) "e38386e382b9e383883ae38386e382b9e3838831: 687474703a2f2f7777772e6578616d706c652e636f6d2fe38386e382b9e383882f, e38386e382b9e38388"
  [1]=>
  string(60) "e38386e382b9e383883ae38386e382b9e3838832: e38386e382b9e38388"
  [2]=>
  string(42) "e38386e382b9e383883ae38386e382b9e3838833: "
}
