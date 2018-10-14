--TEST--
User-space streams
--FILE--
<?php
# vim600:syn=php:

/* This is a fairly aggressive test that looks at
 * user streams and also gives the seek/gets/buffer
 * layer of streams a thorough testing */

$lyrics = <<<EOD
...and the road becomes my bride
I have stripped of all but pride
so in her I do confide
and she keeps me satisfied
gives me all I need
...and with dust in throat I crave
to the game you stay a slave
rover  wanderer
nomad  vagabond
call me what you will
   But Ill take my time anywhere
   Free to speak my mind anywhere
   and Ill redefine anywhere
      Anywhere I roam
	  Where I lay my head is home
...and the earth becomes my throne
I adapt to the unknown
under wandering stars Ive grown
by myself but not alone
I ask no one
...and my ties are severed clean
the less I have the more I gain
off the beaten path I reign
rover  wanderer
nomad  vagabond
call me what you will
   But Ill take my time anywhere
   Free to speak my mind anywhere
   and Ill never mind anywhere
      Anywhere I roam
	  Where I lay my head is home
   But Ill take my time anywhere
   Free to speak my mind anywhere
   and Ill take my find anywhere
      Anywhere I roam
	  Where I lay my head is home
   carved upon my stone
   my body lie but still I roam
      Wherever I may roam.

Wherever I May Roam

EOD;

/* repeat the data a few times so that it grows larger than
 * the default cache chunk size and that we have something
 * to seek around... */
$DATA = "";
for ($i = 0; $i < 30; $i++) {
	if ($i % 2 == 0)
		$DATA .= str_rot13($lyrics);
	else
		$DATA .= $lyrics;
}

/* store the data in a regular file so that we can compare
 * the results */
$tf = tmpfile();
fwrite($tf, $DATA);
$n = ftell($tf);
rewind($tf) or die("failed to rewind tmp file!");
if (ftell($tf) != 0)
	die("tmpfile is not at start!");
$DATALEN = strlen($DATA);
if ($n != $DATALEN)
	die("tmpfile stored $n bytes; should be $DATALEN!");

class uselessstream
{
}

class mystream
{
	public $path;
	public $mode;
	public $options;

	public $position;
	public $varname;

	function stream_open($path, $mode, $options, &$opened_path)
	{
		$this->path = $path;
		$this->mode = $mode;
		$this->options = $options;

		$split = parse_url($path);
		$this->varname = $split["host"];

		if (strchr($mode, 'a'))
			$this->position = strlen($GLOBALS[$this->varname]);
		else
			$this->position = 0;

		return true;
	}

	function stream_read($count)
	{
		$ret = substr($GLOBALS[$this->varname], $this->position, $count);
		$this->position += strlen($ret);
		return $ret;
	}

	function stream_tell()
	{
		return $this->position;
	}

	function stream_eof()
	{
		return $this->position >= strlen($GLOBALS[$this->varname]);
	}

	function stream_seek($offset, $whence)
	{
		switch($whence) {
			case SEEK_SET:
				if ($offset < strlen($GLOBALS[$this->varname]) && $offset >= 0) {
					$this->position = $offset;
					return true;
				} else {
					return false;
				}
				break;
			case SEEK_CUR:
				if ($offset >= 0) {
					$this->position += $offset;
					return true;
				} else {
					return false;
				}
				break;
			case SEEK_END:
				if (strlen($GLOBALS[$this->varname]) + $offset >= 0) {
					$this->position = strlen($GLOBALS[$this->varname]) + $offset;
					return true;
				} else {
					return false;
				}
				break;
			default:
				return false;
		}
	}

}

if (@stream_wrapper_register("bogus", "class_not_exist")) {
	die("Registered a non-existent class!!!???");
}
echo "Not Registered\n";

if (!stream_wrapper_register("test", "mystream")) {
	die("test wrapper registration failed");
}
echo "Registered\n";

if (!stream_wrapper_register("bogon", "uselessstream")) {
	die("bogon wrapper registration failed");
}
echo "Registered\n";

$b = @fopen("bogon://url", "rb");
if (is_resource($b)) {
	die("Opened a bogon??");
}

$fp = fopen("test://DATA", "rb");
if (!$fp || !is_resource($fp)) {
	die("Failed to open resource");
}

/* some default seeks that will cause buffer/cache misses */
$seeks = array(
	array(SEEK_SET, 0, 0),
	array(SEEK_CUR, 8450, 8450),
	array(SEEK_CUR, -7904, 546),
	array(SEEK_CUR, 12456, 13002),

	/* end up at BOF so that randomly generated seek offsets
	 * below will know where they are supposed to be */
	array(SEEK_SET, 0, 0)
);

$whence_map = array(
	SEEK_CUR,
	SEEK_SET,
	SEEK_END
);
$whence_names = array(
	SEEK_CUR => "SEEK_CUR",
	SEEK_SET => "SEEK_SET",
	SEEK_END => "SEEK_END"
	);

/* generate some random seek offsets */
$position = 0;
for ($i = 0; $i < 256; $i++) {
	$whence = $whence_map[array_rand($whence_map, 1)];
	switch($whence) {
		case SEEK_SET:
			$offset = rand(0, $DATALEN - 1);
			$position = $offset;
			break;
		case SEEK_END:
			$offset = -rand(0, $DATALEN - 1);
			$position = $DATALEN + $offset;
			break;
		case SEEK_CUR:
			$offset = rand(0, $DATALEN - 1);
			$offset -= $position;
			$position += $offset;
			break;
	}

	$seeks[] = array($whence, $offset, $position);
}

/* we compare the results of fgets using differing line lengths to
 * test the fgets layer also */
$line_lengths = array(1024, 256, 64, 16);
$fail_count = 0;

ob_start();
foreach($line_lengths as $line_length) {
	/* now compare the real stream with the user stream */
	$j = 0;
	rewind($tf);
	rewind($fp);
	foreach($seeks as $seekdata) {
		list($whence, $offset, $position) = $seekdata;

		$rpb = ftell($tf);
		$rr = (int)fseek($tf, $offset, $whence);
		$rpa = ftell($tf);
		$rline = fgets($tf, $line_length);
		(int)fseek($tf, - strlen($rline), SEEK_CUR);

		$upb = ftell($fp);
		$ur = (int)fseek($fp, $offset, $whence);
		$upa = ftell($fp);
		$uline = fgets($fp, $line_length);
		(int)fseek($fp, - strlen($uline), SEEK_CUR);

		printf("\n--[%d] whence=%s offset=%d line_length=%d position_should_be=%d --\n",
			$j, $whence_names[$whence], $offset, $line_length, $position);
		printf("REAL: pos=(%d,%d,%d) ret=%d line[%d]=`%s'\n", $rpb, $rpa, ftell($tf), $rr, strlen($rline), $rline);
		printf("USER: pos=(%d,%d,%d) ret=%d line[%d]=`%s'\n", $upb, $upa, ftell($fp), $ur, strlen($uline), $uline);

		if ($rr != $ur || $rline != $uline || $rpa != $position || $upa != $position) {
			$fail_count++;
			echo "###################################### FAIL!\n";
			$dat = stream_get_meta_data($fp);
			var_dump($dat);
			break;
		}

		$j++;
	}
	if ($fail_count)
		break;
}

if ($fail_count == 0) {
	ob_end_clean();
	echo "SEEK: OK\n";
} else {
	echo "SEEK: FAIL\n";
	ob_end_flush();
}

$fail_count = 0;

fseek($fp, $DATALEN / 2, SEEK_SET);
fseek($tf, $DATALEN / 2, SEEK_SET);

if (ftell($fp) != ftell($tf)) {
	echo "SEEK: positions do not match!\n";
}

$n = 0;
while(!feof($fp)) {
	$uline = fgets($fp, 1024);
	$rline = fgets($tf, 1024);

	if ($uline != $rline) {
		echo "FGETS: FAIL\niter=$n user=$uline [pos=" . ftell($fp) . "]\nreal=$rline [pos=" . ftell($tf) . "]\n";
		$fail_count++;
		break;
	}
}

if ($fail_count == 0) {
	echo "FGETS: OK\n";
}

/* One final test to see if the position is respected when opened for append */
$fp = fopen("test://lyrics", "a+");
rewind($fp);
var_dump(ftell($fp));
$data = fgets($fp);
fclose($fp);
echo $data . "\n";

?>
--EXPECT--
Not Registered
Registered
Registered
SEEK: OK
FGETS: OK
int(0)
...and the road becomes my bride
