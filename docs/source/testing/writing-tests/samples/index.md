# Samples

## capture_stdio_1.phpt

```php

   --TEST--
   Test covering the I/O stdin and stdout streams.
   --DESCRIPTION--
   This tests checks if the output of stdin and stdout I/O streams match the
   expected content.
   --CAPTURE_STDIO--
   STDIN STDERR
   --FILE--
   <?php
   echo "Hello, world. This is sent to the stdout I/O stream\n";
   fwrite(STDERR, "This is error sent to the stderr I/O stream\n");
   ?>
   --EXPECT--
   This is error sent to the stderr I/O stream
```

## capture_stdio_2.phpt

```php

   --TEST--
   Test covering the I/O stdin and stderr streams.
   --DESCRIPTION--
   This tests checks if the output of stdin and stderr I/O streams match the
   expected content.
   --CAPTURE_STDIO--
   STDIN STDOUT
   --FILE--
   <?php
   echo "Hello, world. This is sent to the stdout I/O stream\n";
   fwrite(STDERR, "This is error sent to the stderr I/O stream\n");
   ?>
   --EXPECT--
   Hello, world. This is sent to the stdout I/O stream
```

## capture_stdio_3.phpt

```php

   --TEST--
   Test covering the all standard I/O streams.
   --DESCRIPTION--
   This tests checks if the output of stdin, stdout and stderr I/O streams match
   the expected content.
   --CAPTURE_STDIO--
   STDIN STDOUT STDERR
   --FILE--
   <?php
   echo "Hello, world. This is sent to the stdout I/O stream\n";
   fwrite(STDERR, "This is error sent to the stderr I/O stream\n");
   ?>
   --EXPECT--
   Hello, world. This is sent to the stdout I/O stream
   This is error sent to the stderr I/O stream
```

## clean.php

```php

   <?php
   include_once(__DIR__ . '/imap_include.inc');

   $imap_stream = imap_open($default_mailbox, $username, $password);

   // delete all msgs in default mailbox, i.e INBOX
   $check = imap_check($imap_stream);
   for ($i = 1; $i <= $check->Nmsgs; $i++) {
     imap_delete($imap_stream, $i);
   }

   $mailboxes = imap_getmailboxes($imap_stream, $server, '*');

   foreach($mailboxes as $value) {
     // Only delete mailboxes with our prefix
     if (preg_match('/\{.*?\}INBOX\.(.+)/', $value->name, $match) == 1) {
       if (strlen($match[1]) >= strlen($mailbox_prefix)
       && substr_compare($match[1], $mailbox_prefix, 0, strlen($mailbox_prefix)) == 0) {
         imap_deletemailbox($imap_stream, $value->name);
       }
     }
   }

   imap_close($imap_stream, CL_EXPUNGE);
   ?>
```

## conflicts_1.phpt

```php

   --TEST--
   Test get_headers() function : test with context
   --CONFLICTS--
   server
   --FILE--
   <?php

   include __DIR__."/../../../../sapi/cli/tests/php_cli_server.inc";
   php_cli_server_start('header("X-Request-Method: ".$_SERVER["REQUEST_METHOD"]);');

   $opts = array(
       'http' => array(
       'method' => 'HEAD'
     )
   );

   $context = stream_context_create($opts);
   $headers = get_headers("http://".PHP_CLI_SERVER_ADDRESS, 1, $context);
   echo $headers["X-Request-Method"]."\n";

   stream_context_set_default($opts);
   $headers = get_headers("http://".PHP_CLI_SERVER_ADDRESS, 1);
   echo $headers["X-Request-Method"]."\n";

   echo "Done";
   ?>
   --EXPECT--
   HEAD
   HEAD
   Done
```

## extensions.phpt

```php

   --TEST--
   phpt EXTENSIONS directive with shared extensions
   --DESCRIPTION--
   This test covers the presence of some loaded extensions with a list of additional
   extensions to be loaded when running test.
   --EXTENSIONS--
   curl
   imagick
   tokenizer
   --FILE--
   <?php
   var_dump(extension_loaded(&apos;curl&apos;));
   var_dump(extension_loaded(&apos;imagick&apos;));
   var_dump(extension_loaded(&apos;tokenizer&apos;));
   ?>
   --EXPECT--
   bool(true)
   bool(true)
   bool(true)
```

## file012.phpt

```php

   <?php
     echo "hello world\n";
   ?>
```

## phpdbg_1.phpt

```php

   --TEST--
   Test deleting breakpoints
   --PHPDBG--
   b 4
   b del 0
   b 5
   r
   b del 1
   r
   y
   q
   --EXPECTF--
   [Successful compilation of %s]
   prompt> [Breakpoint #0 added at %s:4]
   prompt> [Deleted breakpoint #0]
   prompt> [Breakpoint #1 added at %s:5]
   prompt> 12
   [Breakpoint #1 at %s:5, hits: 1]
   >00005: echo $i++;
    00006: echo $i++;
    00007:
   prompt> [Deleted breakpoint #1]
   prompt> Do you really want to restart execution? (type y or n): 1234
   [Script ended normally]
   prompt>
   --FILE--
   <?php
   $i = 1;
   echo $i++;
   echo $i++;
   echo $i++;
   echo $i++;
```

## sample001.phpt

```php

   --TEST--
   Test filter_input() with GET and POST data.
   --DESCRIPTION--
   This test covers both valid and invalid usages of
   filter_input() with INPUT_GET and INPUT_POST data
   and several different filter sanitizers.
   --CREDITS--
   Felipe Pena <felipe@php.net>
   --INI--
   precision=14
   --SKIPIF--
   <?php if (!extension_loaded("filter")) die("Skipped: filter extension required."); ?>
   --GET--
   a=<b>test</b>&b=https://example.com
   --POST--
   c=<p>string</p>&d=12345.7
   --FILE--
   <?php
   ini_set('html_errors', false);
   var_dump(filter_input(INPUT_GET, "a", FILTER_SANITIZE_STRIPPED));
   var_dump(filter_input(INPUT_GET, "b", FILTER_SANITIZE_URL));
   var_dump(filter_input(INPUT_GET, "a", FILTER_SANITIZE_SPECIAL_CHARS, array(1,2,3,4,5)));
   var_dump(filter_input(INPUT_GET, "b", FILTER_VALIDATE_FLOAT, new stdClass));
   var_dump(filter_input(INPUT_POST, "c", FILTER_SANITIZE_STRIPPED, array(5,6,7,8)));
   var_dump(filter_input(INPUT_POST, "d", FILTER_VALIDATE_FLOAT));
   var_dump(filter_input(INPUT_POST, "c", FILTER_SANITIZE_SPECIAL_CHARS));
   var_dump(filter_input(INPUT_POST, "d", FILTER_VALIDATE_INT));
   var_dump(filter_var(new stdClass, "d"));
   var_dump(filter_input(INPUT_POST, "c", "", ""));
   var_dump(filter_var("", "", "", "", ""));
   var_dump(filter_var(0, 0, 0, 0, 0));
   echo "Done\n";
   ?>
   --EXPECTF--
   string(4) "test"
   string(19) "https://example.com"
   string(27) "&#60;b&#62;test&#60;/b&#62;"

   Notice: Object of class stdClass could not be converted to int in %ssample001.php on line %d
   bool(false)
   string(6) "string"
   float(12345.7)
   string(29) "&#60;p&#62;string&#60;/p&#62;"
   bool(false)

   Warning: filter_var() expects parameter 2 to be long, string given in %ssample001.php on line %d
   NULL

   Warning: filter_input() expects parameter 3 to be long, string given in %ssample001.php on line %d
   NULL

   Warning: filter_var() expects at most 3 parameters, 5 given in %ssample001.php on line %d
   NULL

   Warning: filter_var() expects at most 3 parameters, 5 given in %ssample001.php on line %d
   NULL
   Done
```

## sample002.phpt

```php

   --TEST--
   Test receipt of cookie data.
   --CREDITS--
   Zoe Slattery zoe@php.net
   # TestFest Munich 2009-05-19
   --COOKIE--
   hello=World;goodbye=MrChips
   --FILE--
   <?php
   var_dump($_COOKIE);
   ?>
   --EXPECT--
   array(2) {
     ["hello"]=>
     string(5) "World"
     ["goodbye"]=>
     string(7) "MrChips"
   }
```

## sample003.phpt

```php

   --TEST--
   session object deserialization
   --SKIPIF--
   <?php include('skipif.inc'); ?>
   --INI--
   session.use_cookies=0
   session.cache_limiter=
   register_globals=1
   session.serialize_handler=php
   session.save_handler=files
   --FILE--
   <?php
   error_reporting(E_ALL);

   class foo {
       public $bar = "ok";
       function method() { $this->yes++; }
   }

   session_id("abtest");
   session_start();
   session_decode('baz|O:3:"foo":2:{s:3:"bar";s:2:"ok";s:3:"yes";i:1;}arr|a:1:{i:3;O:3:"foo":2:{s:3:"bar";s:2:"ok";s:3:"yes";i:1;}}');

   $baz->method();
   $arr[3]->method();

   var_dump($baz);
   var_dump($arr);
   session_destroy();
   --EXPECT--
   object(foo)#1 (2) {
     ["bar"]=>
     string(2) "ok"
     ["yes"]=>
     int(2)
   }
   array(1) {
     [3]=>
     object(foo)#2 (2) {
       ["bar"]=>
       string(2) "ok"
       ["yes"]=>
       int(2)
     }
   }
```

## sample005.phpt

```php

   --TEST--
   SOAP Server 19: compressed request (gzip)
   --SKIPIF--
   <?php
     if (php_sapi_name()=='cli') echo 'skip';
     require_once('skipif2.inc');
     if (!extension_loaded('zlib')) die('skip zlib extension not available');
   ?>
   --INI--
   precision=14
   --GZIP_POST--
   <SOAP-ENV:Envelope
     SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"
     xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
     xmlns:xsd="http://www.w3.org/2001/XMLSchema"
     xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
     xmlns:si="http://soapinterop.org/xsd">
     <SOAP-ENV:Body>
       <ns1:test xmlns:ns1="http://testuri.org" />
     </SOAP-ENV:Body>
   </SOAP-ENV:Envelope>
   --FILE--
   <?php
   function test() {
     return "Hello World";
   }

   $server = new soapserver(null,array('uri'=>"http://testuri.org"));
   $server->addfunction("test");
   $server->handle();
   echo "ok\n";
   ?>
   --EXPECT--
   <?xml version="1.0" encoding="UTF-8"?>
   <SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://testuri.org" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:testResponse><return xsi:type="xsd:string">Hello World</return></ns1:testResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
   ok
```

## sample006.phpt

```php

   --TEST--
   is_uploaded_file() function
   --CREDITS--
   Dave Kelsey <d_kelsey@uk.ibm.com>
   --SKIPIF--
   <?php if (php_sapi_name()=='cli') die('skip'); ?>
   --POST_RAW--
   Content-type: multipart/form-data, boundary=AaB03x

   --AaB03x
   content-disposition: form-data; name="field1"

   Joe Blow
   --AaB03x
   content-disposition: form-data; name="pics"; filename="file1.txt"
   Content-Type: text/plain

   abcdef123456789
   --AaB03x--
   --FILE--
   <?php
   // uploaded file
   var_dump(is_uploaded_file($_FILES['pics']['tmp_name']));

   // not an uploaded file
   var_dump(is_uploaded_file($_FILES['pics']['name']));

   // not an uploaded file
   var_dump(is_uploaded_file('random_filename.txt'));

   // not an uploaded file
   var_dump(is_uploaded_file('__FILE__'));

   // Error cases
   var_dump(is_uploaded_file());
   var_dump(is_uploaded_file('a', 'b'));

   ?>
   --EXPECTF--
   bool(true)
   bool(false)
   bool(false)
   bool(false)

   Warning: is_uploaded_file() expects exactly 1 parameter, 0 given in %s on line %d
   NULL

   Warning: is_uploaded_file() expects exactly 1 parameter, 2 given in %s on line %d
   NULL
```

## sample007.phpt

```php

   --TEST--
   SOAP Server 20: compressed request (deflate)
   --SKIPIF--
   <?php
     if (php_sapi_name()=='cli') echo 'skip';
     require_once('skipif2.inc');
     if (!extension_loaded('zlib')) die('skip zlib extension not available');
   ?>
   --INI--
   precision=14
   --DEFLATE_POST--
   <?xml version="1.0" encoding="ISO-8859-1"?>
   <SOAP-ENV:Envelope
     SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"
     xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
     xmlns:xsd="http://www.w3.org/2001/XMLSchema"
     xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
     xmlns:si="http://soapinterop.org/xsd">
     <SOAP-ENV:Body>
       <ns1:test xmlns:ns1="http://testuri.org" />
     </SOAP-ENV:Body>
   </SOAP-ENV:Envelope>
   --FILE--
   <?php
   function test() {
     return "Hello World";
   }

   $server = new soapserver(null,array('uri'=>"http://testuri.org"));
   $server->addfunction("test");
   $server->handle();
   echo "ok\n";
   ?>
   --EXPECT--
   <?xml version="1.0" encoding="UTF-8"?>
   <SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://testuri.org" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:testResponse><return xsi:type="xsd:string">Hello World</return></ns1:testResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
   ok
```

## sample008.phpt

```php

   --TEST--
   GET/POST/REQUEST Test with input_filter
   --SKIPIF--
   <?php if (!extension_loaded("filter")) die("skip"); ?>
   --POST--
   d=379
   --GET--
   ar[elm1]=1234&ar[elm2]=0660&a=0234
   --FILE--
   <?php
   $ret = filter_input(INPUT_GET, 'a', FILTER_VALIDATE_INT);
   var_dump($ret);

   $ret = filter_input(INPUT_GET, 'a', FILTER_VALIDATE_INT, array('flags'=>FILTER_FLAG_ALLOW_OCTAL));
   var_dump($ret);

   $ret = filter_input(INPUT_GET, 'ar', FILTER_VALIDATE_INT, array('flags'=>FILTER_REQUIRE_ARRAY));
   var_dump($ret);

   $ret = filter_input(INPUT_GET, 'ar', FILTER_VALIDATE_INT, array('flags'=>FILTER_FLAG_ALLOW_OCTAL|FILTER_REQUIRE_ARRAY));
   var_dump($ret);

   ?>
   --EXPECT--
   bool(false)
   int(156)
   array(2) {
     ["elm1"]=>
     int(1234)
     ["elm2"]=>
     bool(false)
   }
   array(2) {
     ["elm1"]=>
     int(1234)
     ["elm2"]=>
     int(432)
   }
```

## sample009.phpt

```php

   --TEST--
   STDIN input
   --FILE--
   <?php
   var_dump(stream_get_contents(STDIN));
   ?>
   --STDIN--
   fooBar
   use this to input some thing to the php script
   --EXPECT--
   string(54) "fooBar
   use this to input some thing to the php script
   "
```

## sample010.phpt

```php

   --TEST--
   getopt#005 (Required values)
   --ARGS--
   --arg value --arg=value -avalue -a=value -a value
   --INI--
   register_argc_argv=On
   variables_order=GPS
   --FILE--
   <?php
     var_dump(getopt("a:", array("arg:")));
   ?>
   --EXPECT--
   array(2) {
     ["arg"]=>
     array(2) {
       [0]=>
       string(5) "value"
       [1]=>
       string(5) "value"
     }
     ["a"]=>
     array(3) {
       [0]=>
       string(5) "value"
       [1]=>
       string(5) "value"
       [2]=>
       string(5) "value"
     }
   }
```

## sample011.phpt

```php

   --TEST--
   Bug #35382 (Comment in end of file produces fatal error)
   --FILEEOF--
   <?php
   eval("echo 'Hello'; // comment");
   echo " World";
   //last line comment
   --EXPECTF--
   Hello World
```

## sample012.phpt

```php

   --TEST--
   sample test for file_external
   --FILE_EXTERNAL--
   files/file012.inc
   --EXPECT--
   hello world
```

## sample013.phpt

```php

   --TEST--
   SQLite2
   --SKIPIF--
   <?php # vim:ft=php
   if (!extension_loaded('pdo') || !extension_loaded('sqlite')) print 'skip'; ?>
   --REDIRECTTEST--
   return array(
     'ENV' => array(
         'PDOTEST_DSN' => 'sqlite2::memory:'
       ),
     'TESTS' => 'ext/pdo/tests'
     );
```

## sample014.phpt

```php

   --TEST--
   MySQL
   --SKIPIF--
   <?php # vim:ft=php
   if (!extension_loaded('pdo') || !extension_loaded('pdo_mysql')) print 'skip not loaded';
   ?>
   --REDIRECTTEST--
   # magic auto-configuration

   $config = array(
     'TESTS' => 'ext/pdo/tests'
   );

   if (false !== getenv('PDO_MYSQL_TEST_DSN')) {
     # user set them from their shell
     $config['ENV']['PDOTEST_DSN'] = getenv('PDO_MYSQL_TEST_DSN');
     $config['ENV']['PDOTEST_USER'] = getenv('PDO_MYSQL_TEST_USER');
     $config['ENV']['PDOTEST_PASS'] = getenv('PDO_MYSQL_TEST_PASS');
     if (false !== getenv('PDO_MYSQL_TEST_ATTR')) {
       $config['ENV']['PDOTEST_ATTR'] = getenv('PDO_MYSQL_TEST_ATTR');
     }
   } else {
     $config['ENV']['PDOTEST_DSN'] = 'mysql:host=localhost;dbname=test';
     $config['ENV']['PDOTEST_USER'] = 'root';
     $config['ENV']['PDOTEST_PASS'] = '';
   }

   return $config;
```

## sample016.phpt

```php

   --TEST--
   Test get variables with CGI binary
   --GET--
   hello=World&goodbye=MrChips
   --CGI--
   --FILE--
   <?php
   var_dump($_GET);
   ?>
   --EXPECT--
   array(2) {
     ["hello"]=>
     string(5) "World"
     ["goodbye"]=>
     string(7) "MrChips"
   }
```

## sample017.phpt

```php

   --TEST--
   PDO Common: Bug #34630 (inserting streams as LOBs)
   --SKIPIF--
   <?php # vim:ft=php
   if (!extension_loaded('pdo')) die('skip');
   $dir = getenv('REDIR_TEST_DIR');
   if (false == $dir) die('skip no driver');
   require_once $dir . 'pdo_test.inc';
   PDOTest::skip();
   ?>
   --FILE--
   <?php
   if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
   require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
   $db = PDOTest::factory();

   $driver = $db->getAttribute(PDO::ATTR_DRIVER_NAME);
   $is_oci = $driver == 'oci';

   if ($is_oci) {
     $db->exec('CREATE TABLE test (id int NOT NULL PRIMARY KEY, val BLOB)');
   } else {
     $db->exec('CREATE TABLE test (id int NOT NULL PRIMARY KEY, val VARCHAR(256))');
   }
   $db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

   $fp = tmpfile();
   fwrite($fp, "I am the LOB data");
   rewind($fp);

   if ($is_oci) {
     /* oracle is a bit different; you need to initiate a transaction otherwise
      * the empty blob will be committed implicitly when the statement is
      * executed */
     $db->beginTransaction();
     $insert = $db->prepare("insert into test (id, val) values (1, EMPTY_BLOB()) RETURNING val INTO :blob");
   } else {
     $insert = $db->prepare("insert into test (id, val) values (1, :blob)");
   }
   $insert->bindValue(':blob', $fp, PDO::PARAM_LOB);
   $insert->execute();
   $insert = null;

   $db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);
   var_dump($db->query("SELECT * from test")->fetchAll(PDO::FETCH_ASSOC));

   ?>
   --XFAIL--
   This bug might be still open on aix5.2-ppc64 and hpux11.23-ia64
   --EXPECT--
   array(1) {
     [0]=>
     array(2) {
       ["id"]=>
       string(1) "1"
       ["val"]=>
       string(17) "I am the LOB data"
     }
   }
```

## sample018.phpt

```php

   --TEST--
   Phar front controller rewrite access denied [cache_list]
   --INI--
   default_charset=UTF-8
   phar.cache_list={PWD}/frontcontroller10.php
   --SKIPIF--
   <?php if (!extension_loaded("phar")) die("skip"); ?>
   --ENV--
   SCRIPT_NAME=/frontcontroller10.php
   REQUEST_URI=/frontcontroller10.php/hi
   PATH_INFO=/hi
   --FILE_EXTERNAL--
   files/frontcontroller4.phar
   --EXPECTHEADERS--
   Content-type: text/html; charset=UTF-8
   Status: 403 Access Denied
   --EXPECT--
   <html>
    <head>
     <title>Access Denied</title>
    </head>
    <body>
     <h1>403 - File /hi Access Denied</h1>
    </body>
   </html>
```

## sample019.phpt

```php

   --TEST--
   bzopen() and invalid parameters
   --SKIPIF--
   <?php if (!extension_loaded("bz2")) print "skip"; ?>
   --FILE--
   <?php

   var_dump(bzopen());
   var_dump(bzopen("", ""));
   var_dump(bzopen("", "r"));
   var_dump(bzopen("", "w"));
   var_dump(bzopen("", "x"));
   var_dump(bzopen("", "rw"));
   var_dump(bzopen("no_such_file", "r"));

   $fp = fopen(__FILE__,"r");
   var_dump(bzopen($fp, "r"));

   echo "Done\n";
   ?>
   --EXPECTF--
   Warning: bzopen() expects exactly 2 parameters, 0 given in %s on line %d
   NULL

   Warning: bzopen(): '' is not a valid mode for bzopen(). Only 'w' and 'r' are supported. in %s on line %d
   bool(false)

   Warning: bzopen(): filename cannot be empty in %s on line %d
   bool(false)

   Warning: bzopen(): filename cannot be empty in %s on line %d
   bool(false)

   Warning: bzopen(): 'x' is not a valid mode for bzopen(). Only 'w' and 'r' are supported. in %s on line %d
   bool(false)

   Warning: bzopen(): 'rw' is not a valid mode for bzopen(). Only 'w' and 'r' are supported. in %s on line %d
   bool(false)

   Warning: bzopen(no_such_file): failed to open stream: No such file or directory in %s on line %d
   bool(false)
   resource(%d) of type (stream)
   Done
```

## sample020.phpt

```php

   --TEST--
   Bug #42082 (NodeList length zero should be empty)
   --FILE--
   <?php
   $doc = new DOMDocument();
   $xpath = new DOMXPath($doc);
   $nodes = $xpath->query('*');
   var_dump($nodes);
   var_dump($nodes->length);
   $length = $nodes->length;
   var_dump(empty($nodes->length), empty($length));

   $doc->loadXML("<element></element>");
   var_dump($doc->firstChild->nodeValue, empty($doc->firstChild->nodeValue), isset($doc->firstChild->nodeValue));
   var_dump(empty($doc->nodeType), empty($doc->firstChild->nodeType))
   ?>
   --EXPECTF--
   object(DOMNodeList)#%d (0) {
   }
   int(0)
   bool(true)
   bool(true)
   string(0) ""
   bool(true)
   bool(true)
   bool(false)
   bool(false)
```

## sample021.phpt

```php

   --TEST--
   Math constants
   --INI--
   precision=14
   --FILE--
   <?php
   $constants = array(
       "M_E",
       "M_LOG2E",
       "M_LOG10E",
       "M_LN2",
       "M_LN10",
       "M_PI",
       "M_PI_2",
       "M_PI_4",
       "M_1_PI",
       "M_2_PI",
       "M_SQRTPI",
       "M_2_SQRTPI",
       "M_LNPI",
       "M_EULER",
       "M_SQRT2",
       "M_SQRT1_2",
       "M_SQRT3"
   );
   foreach($constants as $constant) {
       printf("%-10s: %s\n", $constant, constant($constant));
   }
   ?>
   --EXPECTREGEX--
   M_E       : 2.718281[0-9]*
   M_LOG2E   : 1.442695[0-9]*
   M_LOG10E  : 0.434294[0-9]*
   M_LN2     : 0.693147[0-9]*
   M_LN10    : 2.302585[0-9]*
   M_PI      : 3.141592[0-9]*
   M_PI_2    : 1.570796[0-9]*
   M_PI_4    : 0.785398[0-9]*
   M_1_PI    : 0.318309[0-9]*
   M_2_PI    : 0.636619[0-9]*
   M_SQRTPI  : 1.772453[0-9]*
   M_2_SQRTPI: 1.128379[0-9]*
   M_LNPI    : 1.144729[0-9]*
   M_EULER   : 0.577215[0-9]*
   M_SQRT2   : 1.414213[0-9]*
   M_SQRT1_2 : 0.707106[0-9]*
   M_SQRT3   : 1.732050[0-9]*
```

## sample022.phpt

```php

   --TEST--
   shm_detach() tests
   --SKIPIF--
   <?php if (!extension_loaded("sysvshm")) print "skip"; ?>
   --FILE--
   <?php

   $key = ftok(__DIR__.'/003.phpt', 'q');

   var_dump(shm_detach());
   var_dump(shm_detach(1,1));

   $s = shm_attach($key);

   var_dump(shm_detach($s));
   var_dump(shm_detach($s));
   shm_remove($s);

   var_dump(shm_detach(0));
   var_dump(shm_detach(1));
   var_dump(shm_detach(-1));

   echo "Done\n";
   ?>
   --CLEAN--
   <?php
   $key = ftok(__DIR__."/003.phpt", 'q');
   $s = shm_attach($key);
   shm_remove($s);
   ?>
   --EXPECTF--
   Warning: shm_detach() expects exactly 1 parameter, 0 given in %ssample022.php on line %d
   NULL

   Warning: shm_detach() expects exactly 1 parameter, 2 given in %ssample022.php on line %d
   NULL
   bool(true)

   Warning: shm_detach(): %d is not a valid sysvshm resource in %ssample022.php on line %d
   bool(false)

   Warning: shm_remove(): %d is not a valid sysvshm resource in %ssample022.php on line %d

   Warning: shm_detach() expects parameter 1 to be resource, integer given in %ssample022.php on line %d
   NULL

   Warning: shm_detach() expects parameter 1 to be resource, integer given in %ssample022.php on line %d
   NULL

   Warning: shm_detach() expects parameter 1 to be resource, integer given in %ssample022.php on line %d
   NULL
   Done
```

## sample023.phpt

```php

   --TEST--
   Bug #23894 (sprintf() decimal specifiers problem)
   --FILE--
   <?php
   $a = -12.3456;
   $test = sprintf("%04d", $a);
   var_dump($test, bin2hex($test));
   $test = sprintf("% 13u", $a);
   var_dump($test, bin2hex($test));
   ?>
   --EXPECTREGEX--
   string\(4\) \"-012\"
   string\(8\) \"2d303132\"
   (string\(13\) \"   4294967284\"|string\(20\) \"18446744073709551604\")
   (string\(26\) \"20202034323934393637323834\"|string\(40\) \"3138343436373434303733373039353531363034\")
```

## sample024.phpt

```php

   --TEST--
   DOMDocument::save  Test basic function of save method
   --SKIPIF--
   <?php
   require_once('skipif.inc');
   ?>
   --FILE--
   <?php
   $doc = new DOMDocument('1.0');
   $doc->formatOutput = true;

   $root = $doc->createElement('book');

   $root = $doc->appendChild($root);

   $title = $doc->createElement('title');
   $title = $root->appendChild($title);

   $text = $doc->createTextNode('This is the title');
   $text = $title->appendChild($text);

   $temp_filename = __DIR__.'/DomDocument_save_basic.tmp';

   echo 'Wrote: ' . $doc->save($temp_filename) . ' bytes'; // Wrote: 72 bytes
   ?>
   --CLEAN--
   <?php
     unlink(__DIR__.'/DomDocument_save_basic.tmp');
   ?>
   --EXPECTF--
   Wrote: 72 bytes
```

## sample025.phpt

```php

   --TEST--
   Test imap_append() function : basic functionality
   --SKIPIF--
   <?php
   require_once(__DIR__.'/skipif.inc');
   ?>
   --FILE--
   <?php
   /* Prototype  : bool imap_append  ( resource $imap_stream  , string $mailbox  , string $message  [, string $options  ] )
    * Description: Append a string message to a specified mailbox.
    * Source code: ext/imap/php_imap.c
    */

   echo "*** Testing imap_append() : basic functionality ***\n";

   require_once(__DIR__.'/imap_include.inc');

   echo "Create a new mailbox for test\n";
   $imap_stream = setup_test_mailbox("", 0);
   if (!is_resource($imap_stream)) {
       exit("TEST FAILED: Unable to create test mailbox\n");
   }

   $mb_details = imap_mailboxmsginfo($imap_stream);
   echo "Add a couple of msgs to new mailbox " . $mb_details->Mailbox . "\n";
   var_dump(imap_append($imap_stream, $mb_details->Mailbox
                      , "From: webmaster@something.com\r\n"
                      . "To: info@something.com\r\n"
                      . "Subject: Test message\r\n"
                      . "\r\n"
                      . "this is a test message, please ignore\r\n"
                      ));

   var_dump(imap_append($imap_stream, $mb_details->Mailbox
                      , "From: webmaster@something.com\r\n"
                      . "To: info@something.com\r\n"
                      . "Subject: Another test\r\n"
                      . "\r\n"
                      . "this is another test message, please ignore it too!!\r\n"
                      ));

   $check = imap_check($imap_stream);
   echo "Msg Count after append : ". $check->Nmsgs . "\n";

   echo "List the msg headers\n";
   var_dump(imap_headers($imap_stream));

   imap_close($imap_stream);
   ?>
   --CLEAN--
   <?php
   require_once('clean.inc');
   ?>
   --EXPECTF--
   *** Testing imap_append() : basic functionality ***
   Create a new mailbox for test
   Create a temporary mailbox and add 0 msgs
   .. mailbox '%s' created
   Add a couple of msgs to new mailbox {%s}INBOX.%s
   bool(true)
   bool(true)
   Msg Count after append : 2
   List the msg headers
   array(2) {
     [0]=>
     string(%d) "%w%s       1)%s webmaster@something. Test message (%d chars)"
     [1]=>
     string(%d) "%w%s       2)%s webmaster@something. Another test (%d chars)"
   }
```

## sample026.phpt

```php

   --TEST--
   SPL: ArrayIterator implementing RecursiveIterator
   --FILE--
   <?php

   $array = array(1, 2 => array(21, 22 => array(221, 222), 23 => array(231)), 3);

   $dir = new RecursiveIteratorIterator(new RecursiveArrayIterator($array), RecursiveIteratorIterator::LEAVES_ONLY);

   foreach ($dir as $file) {
       print "$file\n";
   }

   ?>
   ===DONE===
   <?php exit(0); ?>
   --EXPECT--
   1
   21
   221
   222
   231
   3
```

## skipif2.phpt

```php

   <?php
     if (!extension_loaded('soap')) die('skip soap extension not available');
   ?>
```

## skipif.phpt

```php

   <?php
   // This script prints "skip" if condition does not meet.
   if (!extension_loaded("session") && ini_get("enable_dl")) {
     $dlext = (substr(PHP_OS, 0, 3) == "WIN") ? ".dll" : ".so";
     @dl("session$dlext");
   }
   if (!extension_loaded("session")) {
       die("skip Session module not loaded");
   }
   $save_path = ini_get("session.save_path");
   if ($save_path) {
     if (!file_exists($save_path)) {
       die("skip Session save_path doesn't exist");
     }

     if ($save_path && !@is_writable($save_path)) {
       if (($p = strpos($save_path, ';')) !== false) {
         $save_path = substr($save_path, ++$p);
       }
       if (!@is_writable($save_path)) {
         die("skip\n");
       }
     }
   }
   ?>
```

## xfailif.phpt

```php

   --TEST--
   Handling of errors during linking
   --INI--
   opcache.enable=1
   opcache.enable_cli=1
   opcache.optimization_level=-1
   opcache.preload={PWD}/preload_inheritance_error_ind.inc
   --SKIPIF--
   <?php
   require_once('skipif.inc');
   if (getenv('SKIP_ASAN')) die('xfail Startup failure leak');
   ?>
   --FILE--
   <?php
   echo "Foobar\n";
   ?>
   --EXPECTF--
   Fatal error: Declaration of B::foo($bar) must be compatible with A::foo() in %spreload_inheritance_error.inc on line 8
```
