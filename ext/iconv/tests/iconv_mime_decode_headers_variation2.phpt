--TEST--
Test iconv_mime_encode() function : usage variations - Pass different data types to mode arg
--SKIPIF--
<?php
PHP_INT_SIZE == 4 or die('skip');
extension_loaded('iconv') or die('skip');
function_exists('iconv_mime_decode_headers') or die("skip iconv_mime_decode_headers() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : array iconv_mime_decode_headers(string headers [, int mode, string charset])
 * Description: Decodes multiple mime header fields
 * Source code: ext/iconv/iconv.c
 */

/*
 * Pass different data types to $str argument to see how iconv_mime_decode_headers() behaves
 */

echo "*** Testing iconv_mime_decode_headers() : usage variations ***\n";

// Initialise function arguments not being substituted
$headers = <<<EOF
Subject: =?UTF-8?B?QSBTYW1wbGUgVGVzdA==?=
To: example@example.com
Date: Thu, 1 Jan 1970 00:00:00 +0000
Message-Id: <example@example.com>
Received: from localhost (localhost [127.0.0.1]) by localhost
    with SMTP id example for <example@example.com>;
    Thu, 1 Jan 1970 00:00:00 +0000 (UTC)
    (envelope-from example-return-0000-example=example.com@example.com)
Received: (qmail 0 invoked by uid 65534); 1 Thu 2003 00:00:00 +0000

EOF;

$mode = ICONV_MIME_DECODE_CONTINUE_ON_ERROR;
$charset = 'UTF-8';


//get an unset variable
$unset_var = 10;
unset ($unset_var);

// get a class
class classA
{
  public function __toString() {
    return "Class A object";
  }
}

// heredoc string
$heredoc = <<<EOT
hello world
EOT;

// get a resource variable
$fp = fopen(__FILE__, "r");

// unexpected values to be passed to $str argument
$inputs = array(

       // int data
/*1*/  0,
       1,
       12345,
       -2345,

       // float data
/*5*/  10.5,
       -10.5,
       12.3456789000e10,
       12.3456789000E-10,
       .5,

       // null data
/*10*/ NULL,
       null,

       // boolean data
/*12*/ true,
       false,
       TRUE,
       FALSE,

       // empty data
/*16*/ "",
       '',

       // string data
/*18*/ "string",
       'string',
       $heredoc,

       // object data
/*21*/ new classA(),

       // undefined data
/*22*/ @$undefined_var,

       // unset data
/*23*/ @$unset_var,

       // resource variable
/*24*/ $fp
);

// loop through each element of $inputs to check the behavior of iconv_mime_decode_headers()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( iconv_mime_decode_headers($headers, $input, $charset));
  $iterator++;
};

fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing iconv_mime_decode_headers() : usage variations ***

-- Iteration 1 --
array(5) {
  ["Subject"]=>
  string(13) "A Sample Test"
  ["To"]=>
  string(19) "example@example.com"
  ["Date"]=>
  string(30) "Thu, 1 Jan 1970 00:00:00 +0000"
  ["Message-Id"]=>
  string(21) "<example@example.com>"
  ["Received"]=>
  array(2) {
    [0]=>
    string(204) "from localhost (localhost [127.0.0.1]) by localhost with SMTP id example for <example@example.com>; Thu, 1 Jan 1970 00:00:00 +0000 (UTC) (envelope-from example-return-0000-example=example.com@example.com)"
    [1]=>
    string(57) "(qmail 0 invoked by uid 65534); 1 Thu 2003 00:00:00 +0000"
  }
}

-- Iteration 2 --
array(5) {
  ["Subject"]=>
  string(13) "A Sample Test"
  ["To"]=>
  string(19) "example@example.com"
  ["Date"]=>
  string(30) "Thu, 1 Jan 1970 00:00:00 +0000"
  ["Message-Id"]=>
  string(21) "<example@example.com>"
  ["Received"]=>
  array(2) {
    [0]=>
    string(204) "from localhost (localhost [127.0.0.1]) by localhost with SMTP id example for <example@example.com>; Thu, 1 Jan 1970 00:00:00 +0000 (UTC) (envelope-from example-return-0000-example=example.com@example.com)"
    [1]=>
    string(57) "(qmail 0 invoked by uid 65534); 1 Thu 2003 00:00:00 +0000"
  }
}

-- Iteration 3 --
array(5) {
  ["Subject"]=>
  string(13) "A Sample Test"
  ["To"]=>
  string(19) "example@example.com"
  ["Date"]=>
  string(30) "Thu, 1 Jan 1970 00:00:00 +0000"
  ["Message-Id"]=>
  string(21) "<example@example.com>"
  ["Received"]=>
  array(2) {
    [0]=>
    string(204) "from localhost (localhost [127.0.0.1]) by localhost with SMTP id example for <example@example.com>; Thu, 1 Jan 1970 00:00:00 +0000 (UTC) (envelope-from example-return-0000-example=example.com@example.com)"
    [1]=>
    string(57) "(qmail 0 invoked by uid 65534); 1 Thu 2003 00:00:00 +0000"
  }
}

-- Iteration 4 --
array(5) {
  ["Subject"]=>
  string(13) "A Sample Test"
  ["To"]=>
  string(19) "example@example.com"
  ["Date"]=>
  string(30) "Thu, 1 Jan 1970 00:00:00 +0000"
  ["Message-Id"]=>
  string(21) "<example@example.com>"
  ["Received"]=>
  array(2) {
    [0]=>
    string(204) "from localhost (localhost [127.0.0.1]) by localhost with SMTP id example for <example@example.com>; Thu, 1 Jan 1970 00:00:00 +0000 (UTC) (envelope-from example-return-0000-example=example.com@example.com)"
    [1]=>
    string(57) "(qmail 0 invoked by uid 65534); 1 Thu 2003 00:00:00 +0000"
  }
}

-- Iteration 5 --
array(5) {
  ["Subject"]=>
  string(13) "A Sample Test"
  ["To"]=>
  string(19) "example@example.com"
  ["Date"]=>
  string(30) "Thu, 1 Jan 1970 00:00:00 +0000"
  ["Message-Id"]=>
  string(21) "<example@example.com>"
  ["Received"]=>
  array(2) {
    [0]=>
    string(204) "from localhost (localhost [127.0.0.1]) by localhost with SMTP id example for <example@example.com>; Thu, 1 Jan 1970 00:00:00 +0000 (UTC) (envelope-from example-return-0000-example=example.com@example.com)"
    [1]=>
    string(57) "(qmail 0 invoked by uid 65534); 1 Thu 2003 00:00:00 +0000"
  }
}

-- Iteration 6 --
array(5) {
  ["Subject"]=>
  string(13) "A Sample Test"
  ["To"]=>
  string(19) "example@example.com"
  ["Date"]=>
  string(30) "Thu, 1 Jan 1970 00:00:00 +0000"
  ["Message-Id"]=>
  string(21) "<example@example.com>"
  ["Received"]=>
  array(2) {
    [0]=>
    string(204) "from localhost (localhost [127.0.0.1]) by localhost with SMTP id example for <example@example.com>; Thu, 1 Jan 1970 00:00:00 +0000 (UTC) (envelope-from example-return-0000-example=example.com@example.com)"
    [1]=>
    string(57) "(qmail 0 invoked by uid 65534); 1 Thu 2003 00:00:00 +0000"
  }
}

-- Iteration 7 --

Warning: iconv_mime_decode_headers() expects parameter 2 to be integer, float given in %s on line %d
bool(false)

-- Iteration 8 --
array(5) {
  ["Subject"]=>
  string(13) "A Sample Test"
  ["To"]=>
  string(19) "example@example.com"
  ["Date"]=>
  string(30) "Thu, 1 Jan 1970 00:00:00 +0000"
  ["Message-Id"]=>
  string(21) "<example@example.com>"
  ["Received"]=>
  array(2) {
    [0]=>
    string(204) "from localhost (localhost [127.0.0.1]) by localhost with SMTP id example for <example@example.com>; Thu, 1 Jan 1970 00:00:00 +0000 (UTC) (envelope-from example-return-0000-example=example.com@example.com)"
    [1]=>
    string(57) "(qmail 0 invoked by uid 65534); 1 Thu 2003 00:00:00 +0000"
  }
}

-- Iteration 9 --
array(5) {
  ["Subject"]=>
  string(13) "A Sample Test"
  ["To"]=>
  string(19) "example@example.com"
  ["Date"]=>
  string(30) "Thu, 1 Jan 1970 00:00:00 +0000"
  ["Message-Id"]=>
  string(21) "<example@example.com>"
  ["Received"]=>
  array(2) {
    [0]=>
    string(204) "from localhost (localhost [127.0.0.1]) by localhost with SMTP id example for <example@example.com>; Thu, 1 Jan 1970 00:00:00 +0000 (UTC) (envelope-from example-return-0000-example=example.com@example.com)"
    [1]=>
    string(57) "(qmail 0 invoked by uid 65534); 1 Thu 2003 00:00:00 +0000"
  }
}

-- Iteration 10 --
array(5) {
  ["Subject"]=>
  string(13) "A Sample Test"
  ["To"]=>
  string(19) "example@example.com"
  ["Date"]=>
  string(30) "Thu, 1 Jan 1970 00:00:00 +0000"
  ["Message-Id"]=>
  string(21) "<example@example.com>"
  ["Received"]=>
  array(2) {
    [0]=>
    string(204) "from localhost (localhost [127.0.0.1]) by localhost with SMTP id example for <example@example.com>; Thu, 1 Jan 1970 00:00:00 +0000 (UTC) (envelope-from example-return-0000-example=example.com@example.com)"
    [1]=>
    string(57) "(qmail 0 invoked by uid 65534); 1 Thu 2003 00:00:00 +0000"
  }
}

-- Iteration 11 --
array(5) {
  ["Subject"]=>
  string(13) "A Sample Test"
  ["To"]=>
  string(19) "example@example.com"
  ["Date"]=>
  string(30) "Thu, 1 Jan 1970 00:00:00 +0000"
  ["Message-Id"]=>
  string(21) "<example@example.com>"
  ["Received"]=>
  array(2) {
    [0]=>
    string(204) "from localhost (localhost [127.0.0.1]) by localhost with SMTP id example for <example@example.com>; Thu, 1 Jan 1970 00:00:00 +0000 (UTC) (envelope-from example-return-0000-example=example.com@example.com)"
    [1]=>
    string(57) "(qmail 0 invoked by uid 65534); 1 Thu 2003 00:00:00 +0000"
  }
}

-- Iteration 12 --
array(5) {
  ["Subject"]=>
  string(13) "A Sample Test"
  ["To"]=>
  string(19) "example@example.com"
  ["Date"]=>
  string(30) "Thu, 1 Jan 1970 00:00:00 +0000"
  ["Message-Id"]=>
  string(21) "<example@example.com>"
  ["Received"]=>
  array(2) {
    [0]=>
    string(204) "from localhost (localhost [127.0.0.1]) by localhost with SMTP id example for <example@example.com>; Thu, 1 Jan 1970 00:00:00 +0000 (UTC) (envelope-from example-return-0000-example=example.com@example.com)"
    [1]=>
    string(57) "(qmail 0 invoked by uid 65534); 1 Thu 2003 00:00:00 +0000"
  }
}

-- Iteration 13 --
array(5) {
  ["Subject"]=>
  string(13) "A Sample Test"
  ["To"]=>
  string(19) "example@example.com"
  ["Date"]=>
  string(30) "Thu, 1 Jan 1970 00:00:00 +0000"
  ["Message-Id"]=>
  string(21) "<example@example.com>"
  ["Received"]=>
  array(2) {
    [0]=>
    string(204) "from localhost (localhost [127.0.0.1]) by localhost with SMTP id example for <example@example.com>; Thu, 1 Jan 1970 00:00:00 +0000 (UTC) (envelope-from example-return-0000-example=example.com@example.com)"
    [1]=>
    string(57) "(qmail 0 invoked by uid 65534); 1 Thu 2003 00:00:00 +0000"
  }
}

-- Iteration 14 --
array(5) {
  ["Subject"]=>
  string(13) "A Sample Test"
  ["To"]=>
  string(19) "example@example.com"
  ["Date"]=>
  string(30) "Thu, 1 Jan 1970 00:00:00 +0000"
  ["Message-Id"]=>
  string(21) "<example@example.com>"
  ["Received"]=>
  array(2) {
    [0]=>
    string(204) "from localhost (localhost [127.0.0.1]) by localhost with SMTP id example for <example@example.com>; Thu, 1 Jan 1970 00:00:00 +0000 (UTC) (envelope-from example-return-0000-example=example.com@example.com)"
    [1]=>
    string(57) "(qmail 0 invoked by uid 65534); 1 Thu 2003 00:00:00 +0000"
  }
}

-- Iteration 15 --
array(5) {
  ["Subject"]=>
  string(13) "A Sample Test"
  ["To"]=>
  string(19) "example@example.com"
  ["Date"]=>
  string(30) "Thu, 1 Jan 1970 00:00:00 +0000"
  ["Message-Id"]=>
  string(21) "<example@example.com>"
  ["Received"]=>
  array(2) {
    [0]=>
    string(204) "from localhost (localhost [127.0.0.1]) by localhost with SMTP id example for <example@example.com>; Thu, 1 Jan 1970 00:00:00 +0000 (UTC) (envelope-from example-return-0000-example=example.com@example.com)"
    [1]=>
    string(57) "(qmail 0 invoked by uid 65534); 1 Thu 2003 00:00:00 +0000"
  }
}

-- Iteration 16 --

Warning: iconv_mime_decode_headers() expects parameter 2 to be integer, string given in %s on line %d
bool(false)

-- Iteration 17 --

Warning: iconv_mime_decode_headers() expects parameter 2 to be integer, string given in %s on line %d
bool(false)

-- Iteration 18 --

Warning: iconv_mime_decode_headers() expects parameter 2 to be integer, string given in %s on line %d
bool(false)

-- Iteration 19 --

Warning: iconv_mime_decode_headers() expects parameter 2 to be integer, string given in %s on line %d
bool(false)

-- Iteration 20 --

Warning: iconv_mime_decode_headers() expects parameter 2 to be integer, string given in %s on line %d
bool(false)

-- Iteration 21 --

Warning: iconv_mime_decode_headers() expects parameter 2 to be integer, object given in %s on line %d
bool(false)

-- Iteration 22 --
array(5) {
  ["Subject"]=>
  string(13) "A Sample Test"
  ["To"]=>
  string(19) "example@example.com"
  ["Date"]=>
  string(30) "Thu, 1 Jan 1970 00:00:00 +0000"
  ["Message-Id"]=>
  string(21) "<example@example.com>"
  ["Received"]=>
  array(2) {
    [0]=>
    string(204) "from localhost (localhost [127.0.0.1]) by localhost with SMTP id example for <example@example.com>; Thu, 1 Jan 1970 00:00:00 +0000 (UTC) (envelope-from example-return-0000-example=example.com@example.com)"
    [1]=>
    string(57) "(qmail 0 invoked by uid 65534); 1 Thu 2003 00:00:00 +0000"
  }
}

-- Iteration 23 --
array(5) {
  ["Subject"]=>
  string(13) "A Sample Test"
  ["To"]=>
  string(19) "example@example.com"
  ["Date"]=>
  string(30) "Thu, 1 Jan 1970 00:00:00 +0000"
  ["Message-Id"]=>
  string(21) "<example@example.com>"
  ["Received"]=>
  array(2) {
    [0]=>
    string(204) "from localhost (localhost [127.0.0.1]) by localhost with SMTP id example for <example@example.com>; Thu, 1 Jan 1970 00:00:00 +0000 (UTC) (envelope-from example-return-0000-example=example.com@example.com)"
    [1]=>
    string(57) "(qmail 0 invoked by uid 65534); 1 Thu 2003 00:00:00 +0000"
  }
}

-- Iteration 24 --

Warning: iconv_mime_decode_headers() expects parameter 2 to be integer, resource given in %s on line %d
bool(false)
Done
