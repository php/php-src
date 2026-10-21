--TEST--
session rfc1867: the upload progress update resolves no classes, keeps the session data intact and leaves no $_SESSION behind
--EXTENSIONS--
session
--SKIPIF--
<?php
include('skipif.inc');
if (!getenv('TEST_PHP_CGI_EXECUTABLE')) {
    die('skip CGI not available');
}
?>
--FILE--
<?php

$dir = __DIR__ . '/rfc1867_no_user_code';
@mkdir($dir);
$sid = 'rfc1867nousercode';
$file = "$dir/sess_$sid";
$worker = "$dir/worker.php";

/* A hand-written record, as session_encode() produces it for the `php` handler:
 * an object with __wakeup(), one with __unserialize(), a Serializable payload
 * (C:), an enum shared through a back-reference (E: and r:), and a class that
 * does not exist. Writing it directly leaves the session module untouched until
 * the upload progress handler runs, as in a real request. */
$record = 'w|O:1:"W":1:{s:1:"a";i:1;}'
        . 'u|O:1:"U":1:{s:1:"a";i:2;}'
        . 'c|C:4:"Cust":7:{payload}'
        . 'e|a:2:{i:0;E:11:"Suit:Hearts";i:1;r:7;}'
        . 'm|O:7:"Missing":1:{s:1:"x";i:3;}';
file_put_contents($file, $record);

/* None of these classes exist while the handler decodes the record: the request
 * script is not even compiled yet. With the real classes, E:11:"Suit:Hearts"
 * fails the decode outright, which destroys the session, and C:4:"Cust" loses
 * its payload. The handler keeps every object as __PHP_Incomplete_Class instead
 * and writes it back unchanged. */
file_put_contents($worker, <<<'PHP'
<?php
$record = getenv('TEST_SESSION_RECORD');
$file = getenv('TEST_SESSION_FILE');

spl_autoload_register(function ($class) { echo "autoload($class)\n"; });

enum Suit: string {
    case Hearts = 'H';
}

class W {
    public $a = 1;
    function __wakeup() {
        echo "W::__wakeup\n";
    }
    function __sleep() {
        echo "W::__sleep\n"; return ['a'];
    }
}

class U {
    public $a = 2;
    function __unserialize(array $d) {
        echo "U::__unserialize\n";
        $this->a = $d['a'];
    }
    function __serialize(): array {
        echo "U::__serialize\n";
        return ['a' => $this->a];
    }
}

class Cust implements Serializable {
    public $x = 'payload';
    function serialize() {
        return $this->x;
    }
    function unserialize($d) {
        $this->x = $d;
    }
}

/* The handler needs a $_SESSION to decode the record into, and registers it in
 * the global symbol table. This script has not called session_start(), so it
 * must not be left behind: it would otherwise expose the session data of
 * whatever id the request body carried. */
var_dump(array_key_exists('_SESSION', $GLOBALS));
var_dump(session_status() === PHP_SESSION_NONE);

echo "--- record after the update:\n";
$after = file_get_contents($file);
var_dump(substr($after, 0, strlen($record)) === $record);
var_dump(substr($after, strlen($record), strlen('upload_progress_key|')));

echo "--- session_start():\n";
session_start();
var_dump(session_id());
var_dump($_SESSION['w']->a, $_SESSION['u']->a, $_SESSION['c']->x);
var_dump($_SESSION['e'][0] === Suit::Hearts, $_SESSION['e'][1] === Suit::Hearts);
var_dump($_SESSION['m']);
var_dump(isset($_SESSION['upload_progress_key']['bytes_processed']));
session_destroy();
PHP);

$boundary = '---------------------------20896060251896012921717172737';
$body = "--$boundary\r\n"
      . "Content-Disposition: form-data; name=\"PHPSESSID\"\r\n\r\n$sid\r\n"
      . "--$boundary\r\n"
      . "Content-Disposition: form-data; name=\"PHP_SESSION_UPLOAD_PROGRESS\"\r\n\r\nkey\r\n"
      . "--$boundary\r\n"
      . "Content-Disposition: form-data; name=\"file1\"; filename=\"file1.txt\"\r\n\r\n1\r\n"
      . "--$boundary--\r\n";

$ini = [
    'error_reporting' => 'E_ALL&~E_DEPRECATED',
    'file_uploads' => '1',
    'upload_max_filesize' => '1024',
    'session.save_path' => $dir,
    'session.name' => 'PHPSESSID',
    'session.use_strict_mode' => '0',
    'session.use_cookies' => '0',
    'session.use_only_cookies' => '0',
    'session.cache_limiter' => '',
    'session.save_handler' => 'files',
    'session.serialize_handler' => 'php',
    'session.upload_progress.enabled' => '1',
    'session.upload_progress.cleanup' => '0',
    'session.upload_progress.prefix' => 'upload_progress_',
    'session.upload_progress.name' => 'PHP_SESSION_UPLOAD_PROGRESS',
    'session.upload_progress.freq' => '1%',
    'session.upload_progress.min_freq' => '0.000000001',
];

/* php-cgi is started with -n, so a shared session module has to be named again. */
$extension_dir = ini_get('extension_dir');
foreach (['session.' . PHP_SHLIB_SUFFIX, 'php_session.' . PHP_SHLIB_SUFFIX] as $module) {
    if ($extension_dir !== false && file_exists("$extension_dir/$module")) {
        $ini['extension_dir'] = $extension_dir;
        $ini['extension'] = $module;
        break;
    }
}

$cmd = [getenv('TEST_PHP_CGI_EXECUTABLE'), '-n', '-q'];
foreach ($ini as $name => $value) {
    $cmd[] = '-d';
    $cmd[] = "$name=$value";
}
$cmd[] = '-f';
$cmd[] = $worker;

$env = getenv();
$env['REDIRECT_STATUS'] = '1';
$env['REQUEST_METHOD'] = 'POST';
$env['CONTENT_TYPE'] = "multipart/form-data; boundary=$boundary";
$env['CONTENT_LENGTH'] = (string) strlen($body);
$env['SCRIPT_FILENAME'] = $worker;
$env['PATH_TRANSLATED'] = $worker;
$env['QUERY_STRING'] = '';
$env['TEST_SESSION_RECORD'] = $record;
$env['TEST_SESSION_FILE'] = $file;

$pipes = [];
$proc = proc_open(
    $cmd,
    [0 => ['pipe', 'r'], 1 => ['pipe', 'w'], 2 => ['pipe', 'w']],
    $pipes,
    $dir,
    $env
);
fwrite($pipes[0], $body);
fclose($pipes[0]);
$out = stream_get_contents($pipes[1]);
$err = stream_get_contents($pipes[2]);
fclose($pipes[1]);
fclose($pipes[2]);
proc_close($proc);

echo $out;
if ($err !== '') {
    echo "--- stderr:\n$err";
}

?>
--CLEAN--
<?php
$dir = __DIR__ . '/rfc1867_no_user_code';
@unlink("$dir/worker.php");
foreach (glob("$dir/sess_*") as $f) {
    unlink($f);
}
@rmdir($dir);
?>
--EXPECTF--
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8

bool(false)
bool(true)
--- record after the update:
bool(true)
string(20) "upload_progress_key|"
--- session_start():
autoload(Missing)
W::__wakeup
U::__unserialize
string(17) "rfc1867nousercode"
int(1)
int(2)
string(7) "payload"
bool(true)
bool(true)
object(__PHP_Incomplete_Class)#%d (2) {
  ["__PHP_Incomplete_Class_Name"]=>
  string(7) "Missing"
  ["x"]=>
  int(3)
}
bool(true)
