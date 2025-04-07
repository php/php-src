--TEST--
GH-15205: UAF when destroying stream after pcre request shutdown
--CREDITS--
nicolaslegland
--FILE--
<?php
// Prime cache
preg_replace('/pattern/', 'replace', 'subject');

class wrapper
{
	public function stream_open($path, $mode, $options, &$opened_path)
	{
		return true;
	}

	public function stream_close()
	{
		echo "Close\n";
		preg_replace('/pattern/', 'replace', 'subject');
		preg_match('/(4)?(2)?\d/', '23456', $matches, PREG_OFFSET_CAPTURE | PREG_UNMATCHED_AS_NULL);
		preg_match('/(4)?(2)?\d/', '23456', $matches, PREG_OFFSET_CAPTURE);
	}

	public $context;
}

stream_wrapper_register('wrapper', 'wrapper');
$handle = fopen('wrapper://', 'rb');
?>
--EXPECT--
Close
