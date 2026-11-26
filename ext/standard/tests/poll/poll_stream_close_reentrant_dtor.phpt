--TEST--
Io\Poll: closing a watched stream or modifying watcher data with re-entrant destructor does not crash
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

// Case 1: destructor of watcher #1 removes watcher #2 on the same closing stream
$pair = pt_new_socket_pair();
$ctx1 = pt_new_stream_poll();
$ctx2 = pt_new_stream_poll();
$h1 = new StreamPollHandle($pair[0]);
$h2 = new StreamPollHandle($pair[0]);
$dtor = new class {
    public $w2;
    public function __destruct() {
        echo "case1 dtor\n";
        if ($this->w2) {
            var_dump($this->w2->isActive());
            $this->w2->remove();
        }
    }
};
$ctx1->add($h1, [Io\Poll\Event::Read], $dtor);
$dtor->w2 = $ctx2->add($h2, [Io\Poll\Event::Read]);
unset($dtor);
fclose($pair[0]);
fclose($pair[1]);
echo "case1 ok\n";

// Case 2: destructor of watcher #1 releases context #2 owning watcher #2
$pair = pt_new_socket_pair();
$ctx1 = pt_new_stream_poll();
$ctx2 = pt_new_stream_poll();
$h1 = new StreamPollHandle($pair[0]);
$h2 = new StreamPollHandle($pair[0]);
$dtor = new class($ctx2) {
    public function __construct(public &$ctx2) {}
    public function __destruct() {
        echo "case2 dtor\n";
        $this->ctx2 = null;
    }
};
$ctx1->add($h1, [Io\Poll\Event::Read], $dtor);
$ctx2->add($h2, [Io\Poll\Event::Read]);
unset($dtor);
fclose($pair[0]);
fclose($pair[1]);
echo "case2 ok\n";

// Case 3: modifyData() with a re-entrant destructor reading and updating watcher data
$pair = pt_new_socket_pair();
$ctx = pt_new_stream_poll();
$h = new StreamPollHandle($pair[0]);
$dtor = new class {
    public $watcher;
    public function __destruct() {
        echo "case3 dtor: " . var_export($this->watcher->getData(), true) . "\n";
        $this->watcher->modifyData('from_dtor');
    }
};
$watcher = $ctx->add($h, [Io\Poll\Event::Read], $dtor);
$dtor->watcher = $watcher;
unset($dtor);
$watcher->modifyData('new');
var_dump($watcher->getData());
$watcher->remove();
fclose($pair[0]);
fclose($pair[1]);
echo "case3 ok\n";
?>
--EXPECT--
case1 dtor
bool(false)
case1 ok
case2 dtor
case2 ok
case3 dtor: 'new'
string(9) "from_dtor"
case3 ok
