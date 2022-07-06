--TEST--
Bug #77535 (Invalid callback, h2 server push)
--XFAIL--
http2.golang.org/serverpush is gone
--SKIPIF--
<?php
include 'skipif.inc';
if (getenv("SKIP_ONLINE_TESTS")) {
    die("skip online test");
}
$curl_version = curl_version();
if ($curl_version['version_number'] < 0x073d00) {
    exit("skip: test may crash with curl < 7.61.0");
}
die("skip test is slow due to timeout, and XFAILs anyway");
?>
--FILE--
<?php
class MyHttpClient
{
    private $mh;
    private $curl;

    public function sendRequest()
    {
        if (false === $this->mh = curl_multi_init()) {
            throw new \RuntimeException('Unable to create a new cURL multi handle');
        }

        $this->addServerPushCallback();

        $this->curl = curl_init();
        curl_setopt($this->curl, CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
        curl_setopt($this->curl, CURLOPT_REDIR_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
        curl_setopt($this->curl, CURLOPT_HEADER, false);
        curl_setopt($this->curl, CURLOPT_RETURNTRANSFER, false);
        curl_setopt($this->curl, CURLOPT_FAILONERROR, false);
        curl_setopt($this->curl, CURLOPT_URL, 'https://http2.golang.org/serverpush');
        curl_setopt($this->curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
        curl_setopt($this->curl, CURLOPT_HEADERFUNCTION, function ($ch, $data) {
            return \strlen($data);
        });
        curl_setopt($this->curl, CURLOPT_WRITEFUNCTION, function ($ch, $data) {
            return \strlen($data);
        });
        curl_multi_add_handle($this->mh, $this->curl);

        $stillRunning = null;
        while (true) {
            do {
                $mrc = curl_multi_exec($this->mh, $stillRunning);
            } while (CURLM_CALL_MULTI_PERFORM === $mrc);

            $info = curl_multi_info_read($this->mh);
            while (false !== $info && $info['msg'] == CURLMSG_DONE) {
                if (CURLMSG_DONE !== $info['msg']) {
                    continue;
                }
                echo "Start handle request.\n";
                return;
            }
        }
    }

    private function addServerPushCallback(): void
    {

        $callback = static function () {
            return CURL_PUSH_OK;
        };

        curl_multi_setopt($this->mh, CURLMOPT_PIPELINING, CURLPIPE_MULTIPLEX);
        curl_multi_setopt($this->mh, CURLMOPT_PUSHFUNCTION, $callback);
    }
}

$buzz = new MyHttpClient();
$buzz->sendRequest();
?>
--EXPECT--
Start handle request.

