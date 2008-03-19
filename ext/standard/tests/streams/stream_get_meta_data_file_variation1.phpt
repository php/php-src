--TEST--
stream_get_meta_data() with differing file access modes
--FILE--
<?php

// array of all file access modes
$filemodes = array('r', 'r+', 'w', 'w+', 'a', 'a+', 'x', 'x+',
                   'rb', 'rb+', 'wb', 'wb+', 'ab', 'ab+', 'xb', 'xb+',
                   'rt', 'rt+', 'wt', 'wt+', 'at', 'at+', 'xt', 'xt+');

//create a file
$filename = __FILE__ . '.tmp';
$fp = fopen($filename, 'w+');
fclose($fp);

// open file in each access mode and get meta data
foreach ($filemodes as $mode) {
	if (strncmp($mode, 'x', 1) == 0) {
		// x modes require that file does not exist	
		unlink($filename);
	}
	$fp = fopen($filename, $mode);
	var_dump(stream_get_meta_data($fp));
	fclose($fp);
}

unlink($filename);

?>
--EXPECTF--
array(10) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(1) "r"
  ["unread_bytes"]=>
  int(0)
  ["unread_chars"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(%i) "%s.tmp"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}
array(10) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(2) "r+"
  ["unread_bytes"]=>
  int(0)
  ["unread_chars"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(%i) "%s.tmp"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}
array(10) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(1) "w"
  ["unread_bytes"]=>
  int(0)
  ["unread_chars"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(%i) "%s.tmp"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}
array(10) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(2) "w+"
  ["unread_bytes"]=>
  int(0)
  ["unread_chars"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(%i) "%s.tmp"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}
array(10) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(1) "a"
  ["unread_bytes"]=>
  int(0)
  ["unread_chars"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(%i) "%s.tmp"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}
array(10) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(2) "a+"
  ["unread_bytes"]=>
  int(0)
  ["unread_chars"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(%i) "%s.tmp"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}
array(10) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(1) "x"
  ["unread_bytes"]=>
  int(0)
  ["unread_chars"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(%i) "%s.tmp"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}
array(10) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(2) "x+"
  ["unread_bytes"]=>
  int(0)
  ["unread_chars"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(%i) "%s.tmp"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}
array(10) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(2) "rb"
  ["unread_bytes"]=>
  int(0)
  ["unread_chars"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(%i) "%s.tmp"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}
array(10) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(3) "rb+"
  ["unread_bytes"]=>
  int(0)
  ["unread_chars"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(%i) "%s.tmp"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}
array(10) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(2) "wb"
  ["unread_bytes"]=>
  int(0)
  ["unread_chars"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(%i) "%s.tmp"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}
array(10) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(3) "wb+"
  ["unread_bytes"]=>
  int(0)
  ["unread_chars"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(%i) "%s.tmp"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}
array(10) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(2) "ab"
  ["unread_bytes"]=>
  int(0)
  ["unread_chars"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(%i) "%s.tmp"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}
array(10) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(3) "ab+"
  ["unread_bytes"]=>
  int(0)
  ["unread_chars"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(%i) "%s.tmp"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}
array(10) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(2) "xb"
  ["unread_bytes"]=>
  int(0)
  ["unread_chars"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(%i) "%s.tmp"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}
array(10) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(3) "xb+"
  ["unread_bytes"]=>
  int(0)
  ["unread_chars"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(%i) "%s.tmp"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}
array(10) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(2) "rt"
  ["unread_bytes"]=>
  int(0)
  ["unread_chars"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(%i) "%s.tmp"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}
array(10) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(3) "rt+"
  ["unread_bytes"]=>
  int(0)
  ["unread_chars"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(%i) "%s.tmp"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}
array(10) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(2) "wt"
  ["unread_bytes"]=>
  int(0)
  ["unread_chars"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(%i) "%s.tmp"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}
array(10) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(3) "wt+"
  ["unread_bytes"]=>
  int(0)
  ["unread_chars"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(%i) "%s.tmp"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}
array(10) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(2) "at"
  ["unread_bytes"]=>
  int(0)
  ["unread_chars"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(%i) "%s.tmp"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}
array(10) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(3) "at+"
  ["unread_bytes"]=>
  int(0)
  ["unread_chars"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(%i) "%s.tmp"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}
array(10) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(2) "xt"
  ["unread_bytes"]=>
  int(0)
  ["unread_chars"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(%i) "%s.tmp"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}
array(10) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(3) "xt+"
  ["unread_bytes"]=>
  int(0)
  ["unread_chars"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(%i) "%s.tmp"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}
--UEXPECTF--
array(10) {
  [u"wrapper_type"]=>
  unicode(9) "plainfile"
  [u"stream_type"]=>
  unicode(5) "STDIO"
  [u"mode"]=>
  unicode(1) "r"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(%i) "%s.tmp"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}
array(10) {
  [u"wrapper_type"]=>
  unicode(9) "plainfile"
  [u"stream_type"]=>
  unicode(5) "STDIO"
  [u"mode"]=>
  unicode(2) "r+"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(%i) "%s.tmp"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}
array(10) {
  [u"wrapper_type"]=>
  unicode(9) "plainfile"
  [u"stream_type"]=>
  unicode(5) "STDIO"
  [u"mode"]=>
  unicode(1) "w"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(%i) "%s.tmp"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}
array(10) {
  [u"wrapper_type"]=>
  unicode(9) "plainfile"
  [u"stream_type"]=>
  unicode(5) "STDIO"
  [u"mode"]=>
  unicode(2) "w+"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(%i) "%s.tmp"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}
array(10) {
  [u"wrapper_type"]=>
  unicode(9) "plainfile"
  [u"stream_type"]=>
  unicode(5) "STDIO"
  [u"mode"]=>
  unicode(1) "a"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(%i) "%s.tmp"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}
array(10) {
  [u"wrapper_type"]=>
  unicode(9) "plainfile"
  [u"stream_type"]=>
  unicode(5) "STDIO"
  [u"mode"]=>
  unicode(2) "a+"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(%i) "%s.tmp"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}
array(10) {
  [u"wrapper_type"]=>
  unicode(9) "plainfile"
  [u"stream_type"]=>
  unicode(5) "STDIO"
  [u"mode"]=>
  unicode(1) "x"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(%i) "%s.tmp"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}
array(10) {
  [u"wrapper_type"]=>
  unicode(9) "plainfile"
  [u"stream_type"]=>
  unicode(5) "STDIO"
  [u"mode"]=>
  unicode(2) "x+"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(%i) "%s.tmp"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}
array(10) {
  [u"wrapper_type"]=>
  unicode(9) "plainfile"
  [u"stream_type"]=>
  unicode(5) "STDIO"
  [u"mode"]=>
  unicode(2) "rb"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(%i) "%s.tmp"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}
array(10) {
  [u"wrapper_type"]=>
  unicode(9) "plainfile"
  [u"stream_type"]=>
  unicode(5) "STDIO"
  [u"mode"]=>
  unicode(3) "rb+"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(%i) "%s.tmp"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}
array(10) {
  [u"wrapper_type"]=>
  unicode(9) "plainfile"
  [u"stream_type"]=>
  unicode(5) "STDIO"
  [u"mode"]=>
  unicode(2) "wb"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(%i) "%s.tmp"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}
array(10) {
  [u"wrapper_type"]=>
  unicode(9) "plainfile"
  [u"stream_type"]=>
  unicode(5) "STDIO"
  [u"mode"]=>
  unicode(3) "wb+"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(%i) "%s.tmp"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}
array(10) {
  [u"wrapper_type"]=>
  unicode(9) "plainfile"
  [u"stream_type"]=>
  unicode(5) "STDIO"
  [u"mode"]=>
  unicode(2) "ab"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(%i) "%s.tmp"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}
array(10) {
  [u"wrapper_type"]=>
  unicode(9) "plainfile"
  [u"stream_type"]=>
  unicode(5) "STDIO"
  [u"mode"]=>
  unicode(3) "ab+"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(%i) "%s.tmp"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}
array(10) {
  [u"wrapper_type"]=>
  unicode(9) "plainfile"
  [u"stream_type"]=>
  unicode(5) "STDIO"
  [u"mode"]=>
  unicode(2) "xb"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(%i) "%s.tmp"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}
array(10) {
  [u"wrapper_type"]=>
  unicode(9) "plainfile"
  [u"stream_type"]=>
  unicode(5) "STDIO"
  [u"mode"]=>
  unicode(3) "xb+"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(%i) "%s.tmp"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}
array(11) {
  [u"wrapper_type"]=>
  unicode(9) "plainfile"
  [u"stream_type"]=>
  unicode(5) "STDIO"
  [u"mode"]=>
  unicode(2) "rt"
  [u"read_filters"]=>
  array(1) {
    [0]=>
    unicode(18) "unicode.from.UTF-8"
  }
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(%i) "%s.tmp"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}
array(12) {
  [u"wrapper_type"]=>
  unicode(9) "plainfile"
  [u"stream_type"]=>
  unicode(5) "STDIO"
  [u"mode"]=>
  unicode(3) "rt+"
  [u"read_filters"]=>
  array(1) {
    [0]=>
    unicode(18) "unicode.from.UTF-8"
  }
  [u"write_filters"]=>
  array(1) {
    [0]=>
    unicode(16) "unicode.to.UTF-8"
  }
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(%i) "%s.tmp"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}
array(11) {
  [u"wrapper_type"]=>
  unicode(9) "plainfile"
  [u"stream_type"]=>
  unicode(5) "STDIO"
  [u"mode"]=>
  unicode(2) "wt"
  [u"write_filters"]=>
  array(1) {
    [0]=>
    unicode(16) "unicode.to.UTF-8"
  }
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(%i) "%s.tmp"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}
array(12) {
  [u"wrapper_type"]=>
  unicode(9) "plainfile"
  [u"stream_type"]=>
  unicode(5) "STDIO"
  [u"mode"]=>
  unicode(3) "wt+"
  [u"read_filters"]=>
  array(1) {
    [0]=>
    unicode(18) "unicode.from.UTF-8"
  }
  [u"write_filters"]=>
  array(1) {
    [0]=>
    unicode(16) "unicode.to.UTF-8"
  }
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(%i) "%s.tmp"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}
array(11) {
  [u"wrapper_type"]=>
  unicode(9) "plainfile"
  [u"stream_type"]=>
  unicode(5) "STDIO"
  [u"mode"]=>
  unicode(2) "at"
  [u"write_filters"]=>
  array(1) {
    [0]=>
    unicode(16) "unicode.to.UTF-8"
  }
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(%i) "%s.tmp"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}
array(12) {
  [u"wrapper_type"]=>
  unicode(9) "plainfile"
  [u"stream_type"]=>
  unicode(5) "STDIO"
  [u"mode"]=>
  unicode(3) "at+"
  [u"read_filters"]=>
  array(1) {
    [0]=>
    unicode(18) "unicode.from.UTF-8"
  }
  [u"write_filters"]=>
  array(1) {
    [0]=>
    unicode(16) "unicode.to.UTF-8"
  }
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(%i) "%s.tmp"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}
array(10) {
  [u"wrapper_type"]=>
  unicode(9) "plainfile"
  [u"stream_type"]=>
  unicode(5) "STDIO"
  [u"mode"]=>
  unicode(2) "xt"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(%i) "%s.tmp"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}
array(12) {
  [u"wrapper_type"]=>
  unicode(9) "plainfile"
  [u"stream_type"]=>
  unicode(5) "STDIO"
  [u"mode"]=>
  unicode(3) "xt+"
  [u"read_filters"]=>
  array(1) {
    [0]=>
    unicode(18) "unicode.from.UTF-8"
  }
  [u"write_filters"]=>
  array(1) {
    [0]=>
    unicode(16) "unicode.to.UTF-8"
  }
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(%i) "%s.tmp"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}
