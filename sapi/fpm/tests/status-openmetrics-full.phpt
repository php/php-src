--TEST--
FPM: OpenMetrics full status with per-worker metrics
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[unconfined]
listen = {{ADDR}}
pm = static
pm.max_children = 2
pm.status_path = /status
EOT;

$tester = new FPM\Tester($cfg);
$tester->start();
$tester->expectLogStartNotices();
$tester->request()->expectEmptyBody();

$response = $tester->request(
    'openmetrics&full',
    [],
    '/status'
);

$body = $response->getBody('application/openmetrics-text; version=1.0.0; charset=utf-8');

if ($body === null) {
    echo "ERROR: Could not get OpenMetrics body\n";
    $tester->terminate();
    $tester->close();
    return;
}

$ok = true;

/* Pool-level metrics should be present */
if (!preg_match('/^# HELP phpfpm_up /m', $body)) {
    echo "ERROR: Missing phpfpm_up metric\n";
    $ok = false;
}

/* Per-worker state metrics with one-hot encoding */
if (!preg_match('/^# HELP phpfpm_process_state /m', $body)) {
    echo "ERROR: Missing phpfpm_process_state HELP\n";
    $ok = false;
}
if (!preg_match('/^# TYPE phpfpm_process_state gauge$/m', $body)) {
    echo "ERROR: Missing phpfpm_process_state TYPE\n";
    $ok = false;
}

/* Each child should have exactly 7 state lines (one per possible state) */
for ($child = 0; $child < 2; $child++) {
    $statePattern = '/phpfpm_process_state\{pool="unconfined",child="' . $child . '",state="[^"]+"\} [01]/';
    preg_match_all($statePattern, $body, $matches);
    if (count($matches[0]) !== 7) {
        echo "ERROR: Expected 7 state lines for child $child, got " . count($matches[0]) . "\n";
        $ok = false;
    }
    /* Exactly one state should be 1 */
    $activePattern = '/phpfpm_process_state\{pool="unconfined",child="' . $child . '",state="[^"]+"\} 1/';
    preg_match_all($activePattern, $body, $matches);
    if (count($matches[0]) !== 1) {
        echo "ERROR: Expected exactly 1 active state for child $child, got " . count($matches[0]) . "\n";
        $ok = false;
    }
}

/* Per-worker numeric metrics */
$workerMetrics = [
    'phpfpm_process_requests'            => 'counter',
    'phpfpm_process_request_duration'    => 'gauge',
    'phpfpm_process_last_request_cpu'    => 'gauge',
    'phpfpm_process_last_request_memory' => 'gauge',
];

foreach ($workerMetrics as $metric => $type) {
    if (!preg_match('/^# HELP ' . $metric . ' /m', $body)) {
        echo "ERROR: Missing $metric HELP\n";
        $ok = false;
    }
    if (!preg_match('/^# TYPE ' . $metric . ' ' . $type . '$/m', $body)) {
        echo "ERROR: Missing $metric TYPE\n";
        $ok = false;
    }
    for ($child = 0; $child < 2; $child++) {
        $pattern = '/' . $metric . '\{pool="unconfined",child="' . $child . '"\} /';
        if (!preg_match($pattern, $body)) {
            echo "ERROR: Missing $metric for child $child\n";
            $ok = false;
        }
    }
}

/* Must end with # EOF */
if (!preg_match('/^# EOF$/m', $body)) {
    echo "ERROR: Missing # EOF\n";
    $ok = false;
}

if ($ok) {
    echo "Done\n";
}

$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();

?>
--EXPECT--
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
